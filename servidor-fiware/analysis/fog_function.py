import json
import joblib
import numpy as np
import sys

# Carregar os modelos e o scaler (salvos pelo script de treino)
# Certifique-se que esses arquivos .pkl estão na pasta 'modelos_salvos'
try:
    scaler = joblib.load('modelos_salvos/scaler.pkl')
    kmeans = joblib.load('modelos_salvos/kmeans_model.pkl')
    random_forest = joblib.load('modelos_salvos/random_forest_model.pkl')
except FileNotFoundError:
    print("ERRO: Arquivos de modelo (.pkl) não encontrados. Execute o 'treinar_modelos.py' primeiro.")
    sys.exit(1)

# Lista de features na ORDEM CORRETA (mesma ordem do treinamento)
features_order = ['temperature', 'humidity', 'co2', 'co', 'tol', 'nh4', 'ace', 'o3']

def process_data(data):
    """
    Função principal que processa os dados da entidade recebida.
    'data' será um objeto JSON enviado pelo FogFlow.
    """
    try:
        # 1. Extrair os valores dos atributos
        # Os dados vêm do Orion (nomes longos)
        attributes = data.get('attributes', {})
        
        # Criar um array numpy na ordem correta para o scaler/modelos
        input_data = []
        for feature_name in features_order:
            if feature_name in attributes:
                input_data.append(attributes[feature_name].get('value'))
            else:
                # Lidar com dados faltantes (se necessário)
                input_data.append(0) # ou np.nan, mas precisa ser tratado
        
        # Converter para array 2D para o scaler/modelos
        data_point = np.array(input_data).reshape(1, -1)

        # 2. Padronizar os dados (usando o MESMO scaler do treino)
        data_point_scaled = scaler.transform(data_point)

        # 3. Executar Modelos
        
        # K-Means (Detecção de Anomalias)
        kmeans_cluster = kmeans.predict(data_point_scaled)[0]
        distance = np.min(kmeans.transform(data_point_scaled))
        
        # Use o mesmo limiar (threshold) ou lógica do seu script de treino
        # Exemplo: (precisa ser o mesmo valor do treino)
        # TODO: Salve o threshold do treino em um arquivo ou defina-o aqui
        anomaly_threshold = 1.5 # Exemplo! Use o valor real.
        is_anomaly = bool(distance > anomaly_threshold) # Converte para booleano

        # Random Forest (Classificação)
        quality_prediction = random_forest.predict(data_point_scaled)[0] # Ex: 'boa', 'moderada', 'ruim'

        # 4. Preparar a saída para o FogFlow/Orion
        output = {
            "qualidade_ar": {
                "type": "Text",
                "value": str(quality_prediction)
            },
            "anomalia_detectada": {
                "type": "Boolean",
                "value": is_anomaly
            }
        }
        return output

    except Exception as e:
        print(f"Erro ao processar dados: {e}", file=sys.stderr)
        return None

# --- Loop principal do FogFlow ---
# O FogFlow envia os dados pela entrada padrão (stdin)
if __name__ == "__main__":
    for line in sys.stdin:
        try:
            # Tenta ler a entidade recebida (uma por linha)
            entity_data = json.loads(line)
            
            # Processa os dados
            result = process_data(entity_data)
            
            # Se o processamento foi bem-sucedido, retorna o JSON para o stdout
            if result:
                print(json.dumps(result))
                sys.stdout.flush() # Garante que a saída seja enviada
        except Exception as e:
            print(f"Erro no loop principal: {e}", file=sys.stderr)