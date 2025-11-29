
import json
import joblib
import numpy as np
from flask import Flask, request, jsonify
import sys

# Carregar os modelos e o scaler
try:
    scaler = joblib.load('modelos_treinados/scaler.pkl')
    kmeans = joblib.load('modelos_treinados/kmeans_model.pkl')
    random_forest = joblib.load('modelos_treinados/random_forest_model.pkl')
except FileNotFoundError:
    sys.exit("ERRO: Arquivos de modelo (.pkl) não encontrados.")

# Lista de features na ORDEM CORRETA
features_order = ['temperature', 'humidity', 'co2', 'co', 'tol', 'nh4', 'ace', 'o3']

app = Flask(__name__)

@app.route('/predict', methods=['POST'])
def predict():
    """
    Endpoint para receber dados de sensores e retornar a predição.
    """
    try:
        data = request.get_json()
        
        # Criar um array numpy na ordem correta
        input_data = []
        for feature_name in features_order:
            input_data.append(data.get(feature_name, 0))
        
        data_point = np.array(input_data).reshape(1, -1)
        data_point_scaled = scaler.transform(data_point)

        # K-Means (Detecção de Anomalias)
        distance = np.min(kmeans.transform(data_point_scaled))
        # Este threshold deve ser o mesmo do treino
        anomaly_threshold = 1.5 
        is_anomaly = bool(distance > anomaly_threshold)

        # Random Forest (Classificação)
        quality_prediction = random_forest.predict(data_point_scaled)[0]

        # Preparar a saída
        output = {
            'qualidade_ar': str(quality_prediction),
            'anomalia_detectada': is_anomaly,
            'distancia_cluster': distance
        }
        return jsonify(output)

    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/health', methods=['GET'])
def health():
    """
    Endpoint de healthcheck.
    """
    return jsonify({'status': 'ok'})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
