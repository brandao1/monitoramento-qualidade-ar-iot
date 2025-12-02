
/*********************************************************************
  Código Combinado ESP32 - Multi-Sensor (DHT, MQ-135, MQ-7, MQ-131, PMS7003, MH-Z19E)
  - Publica dados via MQTT.
  - Expõe dados via WebServer.
  - SALVA E CARREGA CALIBRAÇÃO R0 DA NVS (Preferences)
**********************************************************************/

// --- Bibliotecas ---
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT.h>
#include <MQUnifiedsensor.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <PMS.h>
#include <HardwareSerial.h>
#include <MHZ19.h>
#include <Preferences.h>  
#include "ml_logic.h" 

// --- Configuração de Rede (WiFi e MQTT) ---
const char *ssid = "PedroElivia";
const char *password = "157171geb";
const char *mqtt_server = "192.168.0.3";
const int mqtt_port = 1883;
const char *mqtt_client_id = "esp32-sensor001";
const char *mqtt_topic = "/tccapikey/sensor001/attrs";

// ADIÇÃO: Variáveis para os resultados de ML
bool lastAnomaly = false;
const char* lastAirQuality = "calculando";

// --- Definições de Pinos ---
#define DHT22_PIN 33
#define MQ135_PIN 34
#define MQ131_PIN 21
#define MQ7_PIN 32
#define SENSOR_PWM_PIN 5  // Pino de controle do aquecedor (ex: MQ-7)
#define PMS_RX_PIN 25
#define PMS_TX_PIN 26
#define RXD1_PIN 18            // TX do MH-Z19E
#define TXD1_PIN 19            // RX do MH-Z19E
#define FORCE_CALIBRATE_PIN 0  // Pino para forçar (Botão BOOT/FLASH)

// --- Parâmetros Gerais dos Sensores ---
#define BOARD "ESP-32"
#define SENSOR_VOLTAGE_RESOLUTION 3.3
#define SENSOR_ADC_RESOLUTION 12

// --- Parâmetros Específicos (MQ-135) ---
#define SENSOR_TYPE_MQ135 "MQ-135"
#define MQ135_RATIO_CLEAN_AIR 3.6
#define SENSITITY 200

// --- Parâmetros Específicos (MQ-7) ---
#define SENSOR_TYPE_MQ7 "MQ-7"
#define SENSOR_CLEAN_AIR_RATIO 27.5
const unsigned long HEATING_HIGH_DURATION = 60 * 1000;
const unsigned long HEATING_LOW_DURATION = 90 * 1000;

// --- Parâmetros Específicos (MQ-131) ---
#define SENSOR_TYPE_MQ131 "MQ-131"
#define RATIO_MQ131_CLEAN_AIR 15

// --- Objetos (Sensores e Clientes) ---
HardwareSerial &pmsSerial = Serial2;
PMS pms(pmsSerial);
PMS::DATA data;

HardwareSerial SerialMHZ19(1);
MHZ19 mhz19;

WebServer server(80);
DHT dht22(DHT22_PIN, DHT22);
MQUnifiedsensor MQ135(BOARD, SENSOR_VOLTAGE_RESOLUTION, SENSOR_ADC_RESOLUTION, MQ135_PIN, SENSOR_TYPE_MQ135);
MQUnifiedsensor MQ7(BOARD, SENSOR_VOLTAGE_RESOLUTION, SENSOR_ADC_RESOLUTION, MQ7_PIN, SENSOR_TYPE_MQ7);
MQUnifiedsensor MQ131(BOARD, SENSOR_VOLTAGE_RESOLUTION, SENSOR_ADC_RESOLUTION, MQ131_PIN, SENSOR_TYPE_MQ131);

WiFiClient espClient;
PubSubClient mqttClient(espClient);

Preferences preferences;

// --- Variáveis Globais de Estado ---
bool ledState = false;

// Armazenamento das últimas leituras
float lastTemp = 0.0, lastHum = 0.0, lastCO2 = 0.0, lastCO = 0.0;
float lastAlcohol = 0.0, lastToluene = 0.0, lastNH4 = 0.0, lastAcetone = 0.0;
float lastO3 = 0.0, lastNO2 = 0.0, lastCL2 = 0.0;
float lastPM1_0 = 0.0, lastPM2_5 = 0.0, lastPM10_0 = 0.0;

// Controle de tempo
long lastMsg = 0;
long interval = 10000; 
unsigned long heatingStartTime = 0;
bool isHighVoltageHeating = true;  // Estado do aquecedor do MQ-7

// =====================================================================
// Funções Auxiliares
// =====================================================================

void flashLed(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100); 
  }
}

void setup_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

float readDHTTemperature() {
  float t = dht22.readTemperature();
  return isnan(t) ? -999.0 : t;
}

float readDHTHumidity() {
  float h = dht22.readHumidity();
  return isnan(h) ? -999.0 : h;
}

// =====================================================================
// Funções de Calibração e Setup dos Sensores MQ
// =====================================================================

void calibrateMQ135() {
  Serial.print("Calibrando MQ-135, por favor aguarde.");
  float calcR0 = 0;
  for (int i = 1; i <= 10; i++) {
    MQ135.update();
    calcR0 += MQ135.calibrate(MQ135_RATIO_CLEAN_AIR);
    Serial.print(".");
    Serial.print("\n");
    Serial.print(MQ135.calibrate(MQ135_RATIO_CLEAN_AIR));
    Serial.print(".");
  }

  float finalR0 = calcR0 / 10; 
  MQ135.setR0(finalR0);
  Serial.println("  Calibração concluída!");

  preferences.begin("mq-sensors", false);  // Abre em modo de escrita
  preferences.putFloat("r0_mq135", finalR0);
  preferences.end();
  Serial.print("Novo R0 salvo para MQ-135: ");
  Serial.println(finalR0);

  if (isinf(finalR0)) {
    Serial.println("Aviso: R0 é infinito. Verifique a fiação!");
    flashLed(1);
    calibrateMQ135();
  }
  if (finalR0 == 0) {
    Serial.println("Aviso: R0 é zero. Verifique a fiação!");
    flashLed(1);
    calibrateMQ135();
  }
  flashLed(2);
}

void setupSensorMQ135() {
  MQ135.setRegressionMethod(1);
  MQ135.init();

  preferences.begin("mq-sensors", true);                   // Abre em modo de leitura
  float storedR0 = preferences.getFloat("r0_mq135", 0.0);  // Tenta ler; se não existir, retorna 0.0
  preferences.end();

  // Verifica se o valor é válido ("fora do padrão" = 0.0 ou infinito)
  if (storedR0 == 0.0 || isinf(storedR0)) {
    Serial.println("R0 do MQ-135 não encontrado ou inválido. Iniciando nova calibração...");
    calibrateMQ135();  // Esta função agora também salva o novo valor
  } else {
    Serial.print("Carregando R0 salvo para MQ-135: ");
    Serial.println(storedR0);
    MQ135.setR0(storedR0);  // Aplica o valor salvo
    flashLed(2);            // Pisca para indicar que carregou com sucesso
  }
}

void calibrateSensorMQ7() {
  Serial.print("Calibrando sensor MQ-7... ");
  float calcR0 = 0;
  for (int i = 1; i <= 10; i++) {
    MQ7.update();
    calcR0 += MQ7.calibrate(SENSOR_CLEAN_AIR_RATIO);
    Serial.print("\n");
    Serial.print(MQ7.calibrate(SENSOR_CLEAN_AIR_RATIO));
    Serial.print(".");
  }

  float finalR0 = calcR0 / 10;
  MQ7.setR0(finalR0);
  Serial.println(" Concluído!");

  preferences.begin("mq-sensors", false);
  preferences.putFloat("r0_mq7", finalR0);
  preferences.end();
  Serial.print("Novo R0 salvo para MQ-7: ");
  Serial.println(finalR0);

  if (isinf(finalR0)) {
    Serial.println("Erro: Circuito aberto! Verifique o cabeamento.");
    flashLed(1);
    calibrateSensorMQ7();
  }
  if (finalR0 == 0) {
    Serial.println("Erro: Curto-circuito no pino analógico!");
    flashLed(1);
    calibrateSensorMQ7();
  }
  flashLed(2);
}

void setupSensorMQ7() {
  MQ7.setRegressionMethod(1);
  MQ7.setA(99.042);  
  MQ7.setB(-1.518);
  MQ7.init();
  pinMode(SENSOR_PWM_PIN, OUTPUT);

  preferences.begin("mq-sensors", true);
  float storedR0 = preferences.getFloat("r0_mq7", 0.0);
  preferences.end();

  if (storedR0 == 0.0 || isinf(storedR0)) {
    Serial.println("R0 do MQ-7 não encontrado ou inválido. Iniciando nova calibração...");
    calibrateSensorMQ7();
  } else {
    Serial.print("Carregando R0 salvo para MQ-7: ");
    Serial.println(storedR0);
    MQ7.setR0(storedR0);
    flashLed(2);
  }
}

void iniciarCalibracaoMQ131() {
  Serial.println("===================================");
  Serial.println("MODO DE CALIBRAÇÃO ATIVADO (MQ-131).");
  Serial.println("Certifique-se que o sensor está em AR LIMPO e");
  Serial.println("já foi aquecido por pelo menos 15 minutos.");
  Serial.println("Iniciando calibração (tirando 10 amostras)...");
  float R0_CALIBRADO = 0.0;
  for (int i = 1; i <= 10; i++) {
    MQ131.update();
    R0_CALIBRADO += MQ131.calibrate(RATIO_MQ131_CLEAN_AIR);
    Serial.print("\n");
    Serial.print(MQ131.calibrate(RATIO_MQ131_CLEAN_AIR));
    Serial.print(".");
  }

  float finalR0 = R0_CALIBRADO / 10;
  MQ131.setR0(finalR0);
  Serial.println(" Concluído!");

  preferences.begin("mq-sensors", false);
  preferences.putFloat("r0_mq131", finalR0);
  preferences.end();
  Serial.print("Novo R0 salvo para MQ-131: ");
  Serial.println(finalR0);

  if (isinf(finalR0)) {
    Serial.println("Erro: Circuito aberto! Verifique o cabeamento (MQ-131).");
    flashLed(1);
    iniciarCalibracaoMQ131();
  }
  if (finalR0 == 0) {
    Serial.println("Erro: Curto-circuito no pino analógico (MQ-131)!");
    flashLed(1);
    iniciarCalibracaoMQ131();
  }
  flashLed(2);
}

void setupSensorMQ131() {
  MQ131.setRegressionMethod(1);
  MQ131.init();

  preferences.begin("mq-sensors", true);
  float storedR0 = preferences.getFloat("r0_mq131", 0.0);
  preferences.end();

  if (storedR0 == 0.0 || isinf(storedR0)) {
    Serial.println("R0 do MQ-131 não encontrado ou inválido. Iniciando nova calibração...");
    // iniciarCalibracaoMQ131();
  } else {
    Serial.print("Carregando R0 salvo para MQ-131: ");
    Serial.println(storedR0);
    MQ131.setR0(storedR0);
    flashLed(2);
  }
}

// =====================================================================
// Funções do Servidor Web
// =====================================================================

void handleRoot() {
    char msg[3000];
    snprintf(msg, 3000,
             "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css'>\
    <title>ESP32 Sensor Hub</title>\
    <style>\
    html { font-family: Arial, sans-serif; display: inline-block; margin: 0px auto; text-align: center;}\
    h2 { font-size: 2.0rem; color: #2c3e50; }\
    p { font-size: 1.8rem; margin: 15px; }\
    .units { font-size: 1.0rem; }\
    .sensor-labels { font-size: 1.2rem; vertical-align:middle; padding-bottom: 15px; color: #34495e;}\
    .sensor-container { border: 2px solid #ecf0f1; border-radius: 10px; padding: 10px; margin-bottom: 20px; background-color: #f9f9f9;}\
    </style>\
  </head>\
  <body>\
      <h2>ESP32 Sensor Hub</h2>\
      <div class='sensor-container'>\
        <p><i class='fas fa-thermometer-half' style='color:#e74c3c;'></i> <span class='sensor-labels'>Temperatura</span> <span>%.2f</span> <sup class='units'>&deg;C</sup></p>\
        <p><i class='fas fa-tint' style='color:#3498db;'></i> <span class='sensor-labels'>Umidade</span> <span>%.2f</span> <sup class='units'>&percnt;</sup></p>\
      </div>\
      <div class='sensor-container'>\
        <p><i class='fas fa-smog' style='color:#7f8c8d;'></i> <span class='sensor-labels'>CO2</span> <span>%.2f</span> <sup class='units'>ppm</sup></p>\
        <p><i class='fas fa-burn' style='color:#f39c12;'></i> <span class='sensor-labels'>CO</span> <span>%.2f</span> <sup class='units'>ppm</sup></p>\
        <p><i class='fas fa-flask' style='color:#8e44ad;'></i> <span class='sensor-labels'>Tolueno</span> <span>%.2f</span> <sup class='units'>ppm</sup></p>\
        <p><i class='fas fa-vial' style='color:#27ae60;'></i> <span class='sensor-labels'>NH4</span> <span>%.2f</span> <sup class='units'>ppm</sup></p>\
        <p><i class='fas fa-spray-can' style='color:#c0392b;'></i> <span class='sensor-labels'>Acetona</span> <span>%.2f</span> <sup class='units'>ppm</sup></p>\
        <p><i class='fas fa-cloud' style='color:#c0392b;'></i> <span class='sensor-labels'>O3</span> <span>%.2f</span> <sup class='units'>ppm</sup></p>\
        <p><i class='fas fa-cloud' style='color:#c0392b;'></i> <span class='sensor-labels'>NO2</span> <span>%.2f</span> <sup class='units'>ppm</sup></p>\
        <p><i class='fas fa-cloud' style='color:#c0392b;'></i> <span class='sensor-labels'>CL2</span> <span>%.2f</span> <sup class='units'>ppm</sup></p>\
        <p><i class='fas fa-vial' style='color:#c0392b;'></i> <span class='sensor-labels'>Alcohol</span> <span>%.2f</span> <sup class='units'>ppm</sup></p>\
      </div>\
      <!-- ADIÇÃO: Container para os dados de ML -->\
      <div class='sensor-container'>\
        <p><i class='fas fa-shield-alt' style='color:#27ae60;'></i> <span class='sensor-labels'>Qualidade do Ar</span> <span style='font-weight:bold;'>%s</span></p>\
        <p><i class='fas fa-exclamation-triangle' style='color:#e74c3c;'></i> <span class='sensor-labels'>Anomalia Detectada</span> <span>%s</span></p>\
      </div>\
  </body>\
</html>",
             lastTemp, lastHum, lastCO2, lastCO, lastToluene, lastNH4, lastAcetone, lastO3, lastNO2, lastCL2, lastAlcohol,
             lastAirQuality,
             (lastAnomaly ? "<span style=\\\"color:red; font-weight:bold;\\\">SIM</span>" : "<span style=\\\"color:green;\\\">NÃO</span>")
    );
    server.send(200, "text/html", msg);
}

// =====================================================================
// Funções MQTT
// =====================================================================

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Tentando conexão MQTT...");
    if (mqttClient.connect(mqtt_client_id)) {
      Serial.println("conectado!");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

// =====================================================================
// SETUP
// =====================================================================

void setup(void) {
  // Iniciar Serial Primária (Monitor)
  Serial.begin(460800);
  Serial.print("iniciei \n");

  // Configurar pino de calibração
  pinMode(FORCE_CALIBRATE_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);  
  if (digitalRead(FORCE_CALIBRATE_PIN) == LOW) {
    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    Serial.println("!!! FORÇANDO RECALIBRAÇÃO !!!");
    Serial.println("Limpando valores R0 salvos...");
    preferences.begin("mq-sensors", false);  // Abre em modo de escrita
    preferences.clear();                     // Apaga todas as chaves neste "namespace"
    preferences.end();
    Serial.println("Valores R0 limpos. O ESP irá recalibrar todos os sensores.");

    // Pisca o LED 5 vezes para indicar que a limpeza foi feita
    for (int i = 0; i < 5; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
    }
  }

  // Iniciar Serial 2 (PMS7003)
  pmsSerial.begin(9600, SERIAL_8N1, PMS_RX_PIN, PMS_TX_PIN);
  pms.wakeUp();

  // Iniciar Serial 1 (MH-Z19E)
  SerialMHZ19.begin(9600, SERIAL_8N1, RXD1_PIN, TXD1_PIN);
  mhz19.begin(SerialMHZ19);
  mhz19.autoCalibration(false);
  Serial.println("MH-Z19E: Sensor inicializado.");

  // Iniciar Sensores
  dht22.begin();
  setupSensorMQ7();             
  heatingStartTime = millis();  // Inicia o timer do ciclo do MQ-7
  setupSensorMQ131();           
  setupSensorMQ135();           

  // Conectar WiFi
  setup_wifi();

  // Configurar MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);
  Serial.println("Servidor MQTT configurado.");

  // Iniciar MDNS
  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder iniciado. Acesse por http://esp32.local");
  }

  // Iniciar Servidor Web
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

// =====================================================================
// LOOP
// =====================================================================

void loop(void) {
  // Lida com requisições web
  server.handleClient();

  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  // Timer principal para leitura e envio de dados
  long now = millis();
  if (now - lastMsg > interval) {
    lastMsg = now;
    Serial.println("\nLendo sensores...");

    // --- Leitura DHT22 ---
    lastTemp = readDHTTemperature();
    lastHum = readDHTHumidity();

    // --- Leitura MH-Z19E (CO2) ---
    // (Seu código original estava lendo o CO2 do MQ-135, mas você tem um MH-Z19)
    // int co2_ppm = mhz19.getCO2();
    // if (co2_ppm >= 0) {
    //     lastCO2 = (float)co2_ppm;
    //     Serial.print("MH-Z19E CO2: "); Serial.println(lastCO2);
    // } else {
    //     Serial.println("MH-Z19E: Falha ao ler CO2.");
    // }

    // --- Leitura MQ-135 (Vários gases) ---
    // (Mantendo sua lógica de usar o MQ-135 para CO2)
    MQ135.update();
    MQ135.setA(110.47);
    MQ135.setB(-2.862);
    lastCO2 = MQ135.readSensor(false, 0);

    MQ135.setA(44.947);
    MQ135.setB(-3.445);
    lastToluene = MQ135.readSensor(false, 0);

    MQ135.setA(102.2);
    MQ135.setB(-2.473);
    lastNH4 = MQ135.readSensor(false, 0);

    MQ135.setA(34.668);
    MQ135.setB(-3.369);
    lastAcetone = MQ135.readSensor(false, 0);

    MQ135.setA(77.255);
    MQ135.setB(-3.18);
    lastAlcohol = MQ135.readSensor(false, 0);

    MQ135.setA(-462.43);
    MQ135.setB(-2.204);
    lastNO2 = MQ135.readSensor(false, 0);

    MQ135.setA(47.209);
    MQ135.setB(-1.186);
    lastCL2 = MQ135.readSensor(false, 0);

    MQ135.setA(23.943);
    MQ135.setB(-1.11);
    lastO3 = MQ135.readSensor(false, 0);

    // --- Lógica do Aquecedor MQ-7 ---
    if (isHighVoltageHeating) {
      ledcWrite(SENSOR_PWM_PIN, 255);  // 5V (HIGH)
      if (now - heatingStartTime >= HEATING_HIGH_DURATION) {
        isHighVoltageHeating = false;
        heatingStartTime = now;
      }
    } else {
      ledcWrite(SENSOR_PWM_PIN, 75);  // 1.4V (LOW)
      if (now - heatingStartTime >= HEATING_LOW_DURATION) {
        isHighVoltageHeating = true;
        heatingStartTime = now;
      }
    }

    // --- Leitura MQ-7 ---
    MQ7.update();
    lastCO = MQ7.readSensor(false, 0);

    // --- Leitura PMS7003 ---
    pms.requestRead();
    if (pms.readUntil(data, 1000)) {
      lastPM1_0 = data.PM_AE_UG_1_0;
      lastPM2_5 = data.PM_AE_UG_2_5;
      lastPM10_0 = data.PM_AE_UG_10_0;

      // Serial.print("PM1.0: "); Serial.println(lastPM1_0);
      // Serial.print("PM2.5: "); Serial.println(lastPM2_5);
      // Serial.print("PM10: "); Serial.println(lastPM10_0);
      Serial.println("PMS7003: Leitura OK.");
    } else {
      Serial.println("PMS7003: Falha ao ler dados do sensor.");
    }

            
    // 1. Preencher o array 'mlFeatures' NA ORDEM CORRETA do treino
    // Ordem: ['temperature', 'humidity', 'co2', 'co', 'tol', 'nh4', 'ace', 'o3']
    mlFeatures[0] = lastTemp;
    mlFeatures[1] = lastHum;
    mlFeatures[2] = lastCO2;
    mlFeatures[3] = lastCO;
    mlFeatures[4] = lastToluene;
    mlFeatures[5] = lastNH4;
    mlFeatures[6] = lastAcetone;
    mlFeatures[7] = lastO3;
    
    // 2. Executar a inferência
    Serial.println("Executando inferência de ML...");
    MLResult mlResult = runInference();
    
    // 3. Salvar os resultados
    lastAnomaly = mlResult.isAnomaly;
    lastAirQuality = mapClassIndexToName(mlResult.qualityClassIndex);
    
    Serial.print("Resultado ML - Qualidade: "); Serial.println(lastAirQuality);
    Serial.print("Resultado ML - Anomalia: "); Serial.println(lastAnomaly);
    Serial.print("Resultado ML - Distância: "); Serial.println(mlResult.anomalyDistance);
    
    // --- Preparar JSON para MQTT ---
    StaticJsonDocument<384> jsonDoc;

    if (lastTemp > -999.0) jsonDoc["t"] = lastTemp;
    if (lastHum > -999.0) jsonDoc["h"] = lastHum;
    if (lastCO2 >= 0) jsonDoc["co2"] = lastCO2;
    if (lastCO >= 0) jsonDoc["co"] = lastCO;
    if (lastAlcohol >= 0) jsonDoc["oh"] = lastAlcohol;
    if (lastToluene >= 0) jsonDoc["tol"] = lastToluene;
    if (lastNH4 >= 0) jsonDoc["nh4"] = lastNH4;
    if (lastAcetone >= 0) jsonDoc["ace"] = lastAcetone;
    if (lastO3 >= 0) jsonDoc["o3"] = lastO3;
    if (lastNO2 >= 0) jsonDoc["NO2"] = lastNO2;
    if (lastCL2 >= 0) jsonDoc["CL2"] = lastCL2;
    jsonDoc["pm1_0"] = lastPM1_0;
    jsonDoc["pm2_5"] = lastPM2_5;
    jsonDoc["pm10_0"] = lastPM10_0;


    jsonDoc["qualidade_ar"] = lastAirQuality;
    jsonDoc["anomalia"] = lastAnomaly;


    char payload[384];
    serializeJson(jsonDoc, payload);

    // --- Publicar no MQTT ---
    Serial.print("Publicando no MQTT: ");
    Serial.println(payload);
    if (mqttClient.publish(mqtt_topic, payload)) {
      Serial.println("Publicado com sucesso!");
      flashLed(1);
    } else {
      Serial.println("Falha ao publicar!");
      flashLed(2);
    }
  }
}
