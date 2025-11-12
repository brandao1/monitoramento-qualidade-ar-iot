
# Monitoramento de Qualidade do Ar com IoT e Fog Computing

Este projeto apresenta um sistema de monitoramento da qualidade do ar em tempo real, utilizando um microcontrolador ESP32, múltiplos sensores de gás e material particulado, e uma plataforma de fog computing (FogFlow) para processamento e análise de dados com Machine Learning.

## Funcionalidades

- **Coleta de Dados Multi-Sensor:** O ESP32 coleta dados de temperatura, umidade, CO2, CO, tolueno, amônia, acetona, ozônio e material particulado (PM1.0, PM2.5, PM10.0).
- **Comunicação MQTT:** Os dados dos sensores são publicados em um broker MQTT para desacoplamento e escalabilidade.
- **Processamento em Névoa (Fog Computing):** O FogFlow orquestra o fluxo de dados, desde a coleta até a análise.
- **Análise com Machine Learning:**
    - **Detecção de Anomalias:** Um modelo K-Means identifica leituras de sensores anômalas.
    - **Classificação da Qualidade do Ar:** Um modelo Random Forest classifica a qualidade do ar (e.g., boa, moderada, ruim).
- **Armazenamento e Visualização:**
    - Os dados históricos são armazenados no CrateDB através do QuantumLeap.
    - A visualização dos dados em tempo real e históricos é feita com o Grafana.
- **Stack FIWARE:** Utiliza componentes do FIWARE como o Orion Context Broker para gerenciamento de entidades de contexto.

## Arquitetura

1.  **Dispositivo IoT (ESP32):** Coleta os dados dos sensores e os publica no broker MQTT.
2.  **Broker MQTT (Mosquitto):** Recebe os dados do ESP32 e os encaminha para o IoT Agent.
3.  **IoT Agent for JSON:** Atua como um tradutor, convertendo as mensagens MQTT para o formato NGSIv2 e as enviando para o Orion Context Broker.
4.  **Orion Context Broker:** Gerencia o estado atual das entidades (neste caso, o sensor) e notifica o FogFlow sobre as mudanças.
5.  **FogFlow:**
    - **Master Node:** Orquestra as tarefas e o fluxo de dados.
    - **Worker Node:** Executa as "fog functions" (scripts Python) que contêm os modelos de Machine Learning.
6.  **QuantumLeap:** Assina as mudanças no Orion e persiste os dados históricos no CrateDB.
7.  **CrateDB:** Banco de dados de série temporal para armazenamento dos dados históricos.
8.  **Grafana:** Ferramenta de visualização que consulta o CrateDB para exibir dashboards da qualidade do ar.

## Hardware

- ESP32
- Sensor de temperatura e umidade: DHT22
- Sensores de gás: MQ-135, MQ-7, MQ-131
- Sensor de material particulado: PMS7003
- Sensor de CO2: MH-Z19E

## Software e Tecnologias

- **Embarcado:**
    - Arduino Framework (C/C++)
    - Bibliotecas: `PubSubClient`, `ArduinoJson`, `MQUnifiedsensor`, etc.
- **Backend e Fog:**
    - Docker e Docker Compose
    - **FIWARE:** Orion Context Broker, IoT Agent for JSON, QuantumLeap
    - **FogFlow**
    - **MQTT:** Mosquitto
    - **Banco de Dados:** MongoDB, CrateDB, Dgraph
    - **Linguagem (Fog Function):** Python
    - **Bibliotecas de ML (Python):** Scikit-learn (`joblib`), Numpy
- **Visualização:** Grafana

## Setup e Instalação

1.  **Clonar o Repositório:**
    ```bash
    git clone <URL_DO_REPOSITORIO>
    cd <NOME_DO_REPOSITORIO>
    ```

2.  **Configurar o ESP32:**
    - Abra o arquivo `src/monitoramento-esp32.ino` na Arduino IDE.
    - Instale as bibliotecas necessárias através do Library Manager.
    - Altere as credenciais de Wi-Fi (`ssid` e `password`) e o endereço do broker MQTT (`mqtt_server` e `mqtt_port`).
    - Compile e envie o código para o seu ESP32.

3.  **Configurar o Ambiente de Fog/Backend:**
    - Certifique-se de ter o Docker e o Docker Compose instalados.
    - O arquivo `config.json` é usado pelo FogFlow Master. O IP `fogflow-master` é resolvido pelo Docker.
    - Inicie todos os serviços:
      ```bash
      docker-compose up -d
      ```

4.  **Configurar o FogFlow:**
    - Acesse a UI do FogFlow Designer em `http://<SEU_IP>:8090`.
    - Registre os tipos de entidade, os operadores (fog functions) e crie a topologia de serviço para definir o fluxo de dados.

5.  **Configurar o Grafana:**
    - Acesse o Grafana em `http://<SEU_IP>:3000`.
    - Adicione o CrateDB como uma fonte de dados.
    - Crie dashboards para visualizar os dados da qualidade do ar.

## Estrutura de Arquivos

```
.
├── analysis/             # Scripts Python para treino e exportação dos modelos de ML
│   ├── train_export_models.py  # Treina e exporta os modelos
│   ├── fog_function.py         # Função executada no FogFlow para análise
│   └── ...
├── config.json           # Configuração para o FogFlow Master
├── docker-compose.yaml   # Orquestração de todos os serviços backend
├── docs/                 # Documentação adicional
├── mosquitto/            # Configuração e dados do broker MQTT
└── src/
    └── monitoramento-esp32.ino # Código fonte do firmware do ESP32
```

## Uso

Após a configuração, o sistema funcionará automaticamente. O ESP32 enviará os dados dos sensores a cada 10 segundos. O FogFlow processará esses dados, e os resultados (classificação e anomalias) serão atualizados no Orion Context Broker. Os dados históricos podem ser visualizados no Grafana.

## Contribuições

Contribuições são bem-vindas! Se você tiver sugestões para melhorar este projeto, sinta-se à vontade para abrir uma *issue* ou enviar um *pull request*.
