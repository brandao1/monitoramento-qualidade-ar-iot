#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT.h>
#include <MQUnifiedsensor.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *ssid = "PedroElivia";
const char *password = "157171geb";

#define DHT22_PIN 22
#define MQ135_PIN 34
#define MQ131_PIN 35
#define MQ7_PIN 32
#define BOARD "ESP-32"

const char *mqtt_server = "0.tcp.sa.ngrok.io";
const int mqtt_port = 12950;
const char *mqtt_client_id = "esp32-sensor001";
const char *mqtt_topic = "/tccapikey/sensor001/attrs";

#define SENSOR_TYPE_MQ135 "MQ-135"
#define MQ135_RATIO_CLEAN_AIR 3.6

#define SENSOR_VOLTAGE_RESOLUTION 3.3
#define SENSOR_TYPE_MQ7 "MQ-7"
#define SENSOR_ADC_RESOLUTION 12
#define SENSOR_CLEAN_AIR_RATIO 27.5
#define SENSOR_PWM_PIN 5
const unsigned long HEATING_HIGH_DURATION = 60 * 1000;
const unsigned long HEATING_LOW_DURATION = 90 * 1000;

#define SENSOR_TYPE_MQ131 "MQ-131"
#define RATIO_MQ131_CLEAN_AIR 8.9
#define CALC_A_O3 8.9507
#define CALC_B_O3 -1.1601

WebServer server(80);
DHT dht22(DHT22_PIN, DHT22);
MQUnifiedsensor MQ135(BOARD, SENSOR_VOLTAGE_RESOLUTION, SENSOR_ADC_RESOLUTION, MQ135_PIN, SENSOR_TYPE_MQ135);
MQUnifiedsensor MQ7(BOARD, SENSOR_VOLTAGE_RESOLUTION, SENSOR_ADC_RESOLUTION, MQ7_PIN, SENSOR_TYPE_MQ7);
MQUnifiedsensor MQ131(BOARD, SENSOR_VOLTAGE_RESOLUTION, SENSOR_ADC_RESOLUTION, MQ131_PIN, SENSOR_TYPE_MQ131);

WiFiClient espClient;
PubSubClient mqttClient(espClient);

float lastTemp = 0.0;
float lastHum = 0.0;
float lastCO2 = 0.0;
float lastCO = 0.0;
float lastToluene = 0.0;
float lastNH4 = 0.0;
float lastAcetone = 0.0;
float lastO3 = 0.0;

long lastMsg = 0;
long interval = 10000;
unsigned long heatingStartTime = 0;
bool isHighVoltageHeating = true;

float readDHTTemperature() {
    float t = dht22.readTemperature();
    return isnan(t) ? -999.0 : t;
}

float readDHTHumidity() {
    float h = dht22.readHumidity();
    return isnan(h) ? -999.0 : h;
}

void calibrateMQ135() {
    Serial.print("Calibrando MQ-135, por favor aguarde.");
    float calcR0 = 0;
    for(int i = 1; i<=10; i ++) {
        MQ135.update();
        calcR0 += MQ135.calibrate(MQ135_RATIO_CLEAN_AIR);
        Serial.print(".");
        Serial.print("\n");
        Serial.print(MQ135.calibrate(MQ135_RATIO_CLEAN_AIR));
        Serial.print(".");
    }
    MQ135.setR0(calcR0/10);
    Serial.println("  Calibração concluída!");
    if (isinf(calcR0))
    {
        Serial.println("Aviso: R0 é infinito. Verifique a fiação!");
        calibrateMQ135();
    }
    if (calcR0 == 0)
    {
        Serial.println("Aviso: R0 é zero. Verifique a fiação!");
        calibrateMQ135();
    }
}

void setupSensorMQ135() {
     MQ135.setRegressionMethod(1);
    MQ135.init();
    calibrateMQ135();
}

void calibrateSensorMQ7() {
    Serial.print("Calibrando sensor MQ-7... ");
    float calcR0 = 0;
    for (int i = 1; i <= 10; i++)
    {
        MQ7.update();
        calcR0 += MQ7.calibrate(SENSOR_CLEAN_AIR_RATIO);
        Serial.print("\n");
        Serial.print(MQ7.calibrate(SENSOR_CLEAN_AIR_RATIO));
        Serial.print(".");
    }
    MQ7.setR0(calcR0 / 10);
    Serial.println(" Concluído!");
    if (isinf(calcR0))
    {
        Serial.println("Erro: Circuito aberto! Verifique o cabeamento.");
        calibrateSensorMQ7();
    }
    if (calcR0 == 0)
    {
        Serial.println("Erro: Curto-circuito no pino analógico!");
        calibrateSensorMQ7();
    }
}

void setupSensorMQ7() {
    MQ7.setRegressionMethod(1);
    MQ7.setA(99.042);
    MQ7.setB(-1.518);
    MQ7.init();
    pinMode(SENSOR_PWM_PIN, OUTPUT);
    calibrateSensorMQ7();
}

void setupSensorMQ131() {
    MQ131.setRegressionMethod(1);
    MQ131.setA(CALC_A_O3);
    MQ131.setB(CALC_B_O3);
    MQ131.init();
    pinMode(SENSOR_PWM_PIN, OUTPUT);
    iniciarCalibracaoMQ131();
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
    MQ131.setR0(R0_CALIBRADO / 10);
    Serial.println(" Concluído!");
        if (isinf(R0_CALIBRADO))
    {
        Serial.println("Erro: Circuito aberto! Verifique o cabeamento (MQ-131).");
        iniciarCalibracaoMQ131();
    }
    if (R0_CALIBRADO == 0)
    {
        Serial.println("Erro: Curto-circuito no pino analógico (MQ-131)!");
        iniciarCalibracaoMQ131();
    }
}

void handleRoot()
{
    char msg[2500];
    snprintf(msg, 2500,
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
      </div>\
  </body>\
</html>",
             lastTemp, lastHum, lastCO2, lastCO, lastToluene, lastNH4, lastAcetone, lastO3);
    server.send(200, "text/html", msg);
}

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

void setup(void)
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    dht22.begin();
    setupSensorMQ7();
    heatingStartTime = millis();
    setupSensorMQ131();
    setupSensorMQ135();

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Conectando ao WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi conectado!");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());

    mqttClient.setServer(mqtt_server, mqtt_port);
    Serial.println("Servidor MQTT configurado.");

    if (MDNS.begin("esp32"))
    {
        Serial.println("MDNS responder iniciado. Acesse por http://esp32.local");
    }
    server.on("/", handleRoot);
    server.begin();
    Serial.println("Servidor HTTP iniciado");
}

void loop(void)
{
    server.handleClient();

    if (!mqttClient.connected()) {
      reconnectMQTT();
    }
    mqttClient.loop();

    long now = millis();
    if (now - lastMsg > interval)
    {
        lastMsg = now;
        Serial.println("\nLendo sensores...");

        lastTemp = readDHTTemperature();
        lastHum = readDHTHumidity();

        MQ135.update();
        MQ135.setA(110.47); MQ135.setB(-2.862); lastCO2 = MQ135.readSensor();
        MQ135.setA(44.947); MQ135.setB(-3.445); lastToluene = MQ135.readSensor();
        MQ135.setA(102.2);  MQ135.setB(-2.473); lastNH4 = MQ135.readSensor();
        MQ135.setA(34.668); MQ135.setB(-3.369); lastAcetone = MQ135.readSensor();

        if (isHighVoltageHeating) {
            analogWrite(SENSOR_PWM_PIN, 255);
            if (now - heatingStartTime >= HEATING_HIGH_DURATION) {
                isHighVoltageHeating = false; heatingStartTime = now;
            }
        } else {
            analogWrite(SENSOR_PWM_PIN, 75);
            if (now - heatingStartTime >= HEATING_LOW_DURATION) {
                isHighVoltageHeating = true; heatingStartTime = now;
            }
        }
        MQ7.update(); lastCO = MQ7.readSensor();

        MQ131.update(); lastO3 = MQ131.readSensor();

        StaticJsonDocument<256> jsonDoc;

        if (lastTemp > -999.0) jsonDoc["t"] = lastTemp;
        if (lastHum > -999.0) jsonDoc["h"] = lastHum;
        if (lastCO2 >= 0) jsonDoc["co2"] = lastCO2;
        if (lastCO >= 0) jsonDoc["co"] = lastCO;
        if (lastToluene >= 0) jsonDoc["tol"] = lastToluene;
        if (lastNH4 >= 0) jsonDoc["nh4"] = lastNH4;
        if (lastAcetone >= 0) jsonDoc["ace"] = lastAcetone;
        if (lastO3 >= 0) jsonDoc["o3"] = lastO3;

        char payload[256];
        serializeJson(jsonDoc, payload);

        Serial.print("Publicando no MQTT: ");
        Serial.println(payload);
        if (mqttClient.publish(mqtt_topic, payload)) {
          Serial.println("Publicado com sucesso!");
          digitalWrite(LED_BUILTIN, HIGH);
          delay(100);
          digitalWrite(LED_BUILTIN, LOW);
        } else {
          Serial.println("Falha ao publicar!");
        }
    }
}