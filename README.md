# Monitoramento de Qualidade do Ar com IoT e Análise Embarcada (Edge ML)

Este projeto apresenta um sistema de monitoramento da qualidade do ar em tempo real, utilizando um microcontrolador ESP32, múltiplos sensores e **processamento de Machine Learning embarcado (Edge ML)**. Os dados brutos e os resultados da análise são enviados para uma plataforma FIWARE para armazenamento e visualização.

## Funcionalidades

  - **Coleta de Dados Multi-Sensor:** O ESP32 coleta dados de temperatura, umidade, CO2, CO, tolueno, amônia, acetona, ozônio, gases (NO2, CL2) e material particulado (PM1.0, PM2.5, PM10.0).
  - **Comunicação MQTT:** Os dados dos sensores e os resultados da análise são publicados em um broker MQTT.
  - **Análise Embarcada (Edge ML):** Os modelos de Machine Learning são executados **diretamente no ESP32**:
      - **Detecção de Anomalias:** Um modelo K-Means (convertido para C) identifica leituras de sensores anômalas localmente.
      - **Classificação da Qualidade do Ar:** Um modelo Random Forest (convertido para C) classifica a qualidade do ar (e.g., boa, moderada, ruim).
  - **Calibração Persistente:** O ESP32 salva os valores de calibração (R0) dos sensores MQ na memória NVS (flash), evitando a necessidade de recalibrar a cada reinicialização.
  - **Armazenamento e Visualização (Stack FIWARE):**
      - Os dados históricos são armazenados no CrateDB através do QuantumLeap.
      - A visualização dos dados em tempo real e históricos é feita com o Grafana.

## Arquitetura

1.  **Dispositivo IoT (ESP32):**
      * Coleta os dados dos sensores.
      * (Opcional, se implementado no firmware) Executa os modelos de Scaler, K-Means e Random Forest (do `ml_models.h`) para classificar a qualidade do ar e detectar anomalias.
      * Publica os dados brutos e/ou os resultados da análise no broker MQTT.
2.  **Broker MQTT (Mosquitto):** Recebe os dados do ESP32 e os encaminha para o IoT Agent.
3.  **IoT Agent for JSON:** Atua como um tradutor, convertendo as mensagens MQTT para o formato NGSIv2 e as enviando para o Orion Context Broker.
4.  **Orion Context Broker:** Gerencia o estado atual das entidades (o sensor) e notifica o QuantumLeap sobre as mudanças.
5.  **QuantumLeap:** Assina as mudanças no Orion e persiste os dados históricos no CrateDB.
6.  **CrateDB:** Banco de dados de série temporal para armazenamento dos dados históricos.
7.  **Grafana:** Ferramenta de visualização que consulta o CrateDB para exibir dashboards.

## Hardware

  - ESP32
  - Sensor de temperatura e umidade: DHT22
  - Sensores de gás: MQ-135, MQ-7, MQ-131
  - Sensor de material particulado: PMS7003
  - Sensor de CO2: MH-Z19E

## Software e Tecnologias

  - **Embarcado:**
      - Arduino Framework (C/C++)
      - Bibliotecas: `PubSubClient`, `ArduinoJson`, `MQUnifiedsensor`, `Preferences.h`, etc.
      - `ml_models.h` (Arquivo gerado contendo os modelos de ML em C).
  - **Backend (FIWARE):**
      - Docker e Docker Compose
      - **FIWARE:** Orion Context Broker, IoT Agent for JSON, QuantumLeap
      - **MQTT:** Mosquitto
      - **Banco de Dados:** MongoDB, CrateDB
  - **Treinamento de Modelos (Desenvolvimento):**
      - Python
      - Bibliotecas: `scikit-learn`, `joblib`, `pandas`, `numpy`, `m2cgen`, `python-crate`

-----

## Fluxo de Execução e Instalação

Este projeto tem dois fluxos principais: o **Fluxo de Backend** (servidores FIWARE, executados uma vez) e o **Fluxo de Desenvolvimento do Firmware** (treinar modelos, compilar e enviar ao ESP32).

### Passo 1: Iniciar o Backend (FIWARE + BDs)

Os serviços de backend (MQTT, FIWARE, BDs) são gerenciados pelo Docker.

1.  Certifique-se de ter o Docker e o Docker Compose instalados.
2.  Inicie todos os serviços em segundo plano:
    ```bash
    docker-compose up -d
    ```
3.  Verifique se os contêineres estão em execução:
    ```bash
    docker-compose ps
    ```
    (Você deve ver `orion`, `mongo-db`, `cratedb`, `quantumleap`, `grafana`, `iot-agent`, `mosquitto`, etc., todos "Up".)

### Passo 2: Treinar e Exportar os Modelos de ML

Este passo usa os scripts Python na pasta `analysis/` para treinar os modelos de ML com dados do CrateDB e exportá-los para o arquivo `ml_models.h`.

1.  **Pré-requisito:** Você precisa ter dados de sensores já armazenados no CrateDB para que os modelos possam ser treinados. (Se estiver começando do zero, você pode pular este passo e usar o ESP32 apenas para coletar dados brutos primeiro, ou adaptar o script `train_export_porter.py` para ler um CSV local).

2.  Instale as dependências do Python:

    ```bash
    pip install scikit-learn joblib pandas numpy m2cgen python-crate
    ```

3.  Execute o script de **treinamento** (lê do CrateDB e salva os arquivos `.pkl`):

    ```bash
    python train_export_porter.py
    ```

4.  Execute o script de **exportação** (converte os `.pkl` em arquivos `.c` e `.h` intermediários):

    ```bash
    python export_model_to_c.py
    ```

5.  Execute o script de **empacotamento** (combina tudo no arquivo final `ml_models.h`):

    ```bash
    python package_models.py
    ```

Ao final deste passo, você terá um novo arquivo `ml_models.h` na raiz do projeto.

### Passo 3: Configurar e Compilar o Firmware do ESP32

1.  **Mova o Modelo:** Copie o arquivo `ml_models.h` gerado no passo anterior para dentro da pasta do seu projeto Arduino (ex: `src/` ou crie uma aba `ml_models.h` na Arduino IDE e cole o conteúdo).

2.  **Configurar o Código:**

      - Abra o arquivo `monitoramento-esp32.ino` na Arduino IDE ou VSCode (PlatformIO).
      - Instale as bibliotecas necessárias (ex: `PubSubClient`, `ArduinoJson`, `MQUnifiedsensor`, `DHT sensor library`, `PMS`, `MHZ19`).
      - Altere as credenciais de Wi-Fi (`ssid` e `password`).
      - Altere o endereço do broker MQTT (`mqtt_server` e `mqtt_port`) para apontar para o IP da máquina que está rodando o Docker. (Se o ESP32 e o Docker estiverem na mesma rede, use o IP local da máquina, ex: `192.168.1.10`).

3.  **(Opcional) Integrar a Inferência de ML no Loop:**

    > **Nota:** O código `monitoramento-esp32.ino` fornecido foca na coleta de dados e calibração, mas ainda não chama as funções de ML. Para ativar a análise embarcada, você precisaria:

    > a. Incluir o header: `#include "ml_models.h"` no topo do arquivo `.ino`.
    > b. Criar um array `float features[ML_FEATURE_COUNT]` com os dados dos sensores **na ordem correta** definida em `ml_constants.h`.
    > c. **Aplicar o Scaler:** Normalizar os *features* usando `SCALER_MEAN` e `SCALER_SCALE`.
    > d. **Executar o Random Forest:** Chamar `int classe = predict_rf(scaled_features);` para obter a classe (0, 1, 2).
    > e. **Executar o K-Means:** Calcular a distância euclidiana dos `scaled_features` para os `KMEANS_CENTROIDS` e verificar se a menor distância é maior que `KMEANS_ANOMALY_THRESHOLD`.
    > f. Adicionar os resultados (`classe` e `is_anomaly`) ao JSON enviado via MQTT.

4.  **Calibração dos Sensores MQ:**

      - Na **primeira vez** que executar, deixe o ESP32 ligado por 15-20 minutos em ar limpo para que os sensores aqueçam e estabilizem.
      - O código irá automaticamente calibrar os sensores (MQ135, MQ7, MQ131) e salvar os valores R0 na memória NVS.
      - Em reinicializações futuras, ele carregará esses valores automaticamente.
      - **Para forçar uma recalibração:** Mantenha o botão "BOOT" (Pino 0) pressionado enquanto o ESP32 é ligado. O monitor serial mostrará "FORÇANDO RECALIBRAÇÃO".

5.  **Compilar e Enviar:** Compile e envie o código para o seu ESP32.

### Passo 4: Configurar o Grafana

1.  Acesse o Grafana em `http://<IP_DO_DOCKER>:3000` (login padrão: admin/admin).
2.  Adicione uma nova fonte de dados (Data Source):
      - Tipo: `CrateDB`
      - URL: `http://cratedb:4200` (O Grafana dentro do Docker pode resolver o nome `cratedb`)
3.  Crie dashboards para visualizar os dados da tabela `etsensor` no schema `mttcc_service`.

## Estrutura de Arquivos

```
.
├── analysis/             # Scripts Python para treino e exportação dos modelos de ML
│   ├── train_export_porter.py  # 1. Treina e salva .pkl
│   ├── export_model_to_c.py    # 2. Converte .pkl para .c/.h
│   ├── package_models.py       # 3. Empacota tudo em 'ml_models.h'
│   └── ...
├── docker-compose.yaml   # Orquestração de todos os serviços backend (FIWARE, BDs)
├── docs/                 # Documentação adicional
├── mosquitto/            # Configuração e dados do broker MQTT
├── ml_models.h           # ARQUIVO GERADO: Modelos de ML em C (copiar para o projeto Arduino)
└── src/
    └── monitoramento-esp32.ino # Código fonte do firmware do ESP32
```
