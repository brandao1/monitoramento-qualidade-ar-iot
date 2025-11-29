import joblib
import numpy as np
import m2cgen as m2c
import os
import json

MODEL_DIR = "modelos_treinados"

# --------- CARREGAR MODELOS TREINADOS ----------
kmeans_path = os.path.join(MODEL_DIR, "kmeans_model.pkl")
rf_path = os.path.join(MODEL_DIR, "random_forest_model.pkl")
scaler_path = os.path.join(MODEL_DIR, "scaler.pkl")
centroids_json_path = os.path.join(MODEL_DIR, "centroids.json")

kmeans = joblib.load(kmeans_path)
rf = joblib.load(rf_path)
scaler = joblib.load(scaler_path)

with open(centroids_json_path, "r") as f:
    config_data = json.load(f)
    
kmeans_threshold = config_data["threshold"]

# --------- EXPORTAR RANDOM FOREST PARA C (m2cgen) ---------
print("Gerando código C do RandomForest...")
# Obter as classes (ex: 'boa', 'moderada', 'ruim')
# O m2cgen retorna um índice (0, 1, 2)
classes_mapping = rf.classes_
rf_c_code = m2c.export_to_c(rf)

rf_model_c_path = os.path.join(MODEL_DIR, "random_forest_model.c")
with open(rf_model_c_path, "w") as f:
    f.write(rf_c_code)

print(f"Arquivo '{rf_model_c_path}' gerado com sucesso!")

# --------- EXPORTAR CENTROIDES DO KMEANS ---------
print("Gerando centroides do KMeans...")

centroids = kmeans.cluster_centers_
n_clusters, n_features = centroids.shape

kmeans_h_path = os.path.join(MODEL_DIR, "kmeans_centroids.h")
with open(kmeans_h_path, "w") as f:
    f.write(f"static const int KMEANS_CLUSTERS = {n_clusters};\n")
    f.write(f"static const int KMEANS_FEATURES = {n_features};\n")
    f.write("static const float KMEANS_CENTROIDS[KMEANS_CLUSTERS][KMEANS_FEATURES] = {\n")

    for i, centroid in enumerate(centroids):
        arr = ", ".join(f"{x:.6f}f" for x in centroid)
        f.write(f"    {{ {arr} }}")
        if i < n_clusters - 1:
            f.write(",")
        f.write("\n")

    f.write("};\n")

print(f"Arquivo '{kmeans_h_path}' gerado com sucesso!")

# --------- EXPORTAR SCALER e CONSTANTES ---------
print("Gerando constantes do Scaler e KMeans...")

scaler_mean = scaler.mean_
scaler_scale = scaler.scale_

constants_h_path = os.path.join(MODEL_DIR, "ml_constants.h")
with open(constants_h_path, "w") as f:
    f.write(f"// Constantes de Machine Learning exportadas\n\n")
    
    # --- Ordem das Features (IMPORTANTE) ---
    # Deve bater com a ordem do treino em train_export_porter.py
    f.write("// Ordem das features: ['temperature', 'humidity', 'co2', 'co', 'tol', 'nh4', 'ace', 'o3']\n")
    f.write(f"static const int ML_FEATURE_COUNT = {len(scaler_mean)};\n\n")
    
    # --- Scaler ---
    mean_arr = ", ".join(f"{x:.6f}f" for x in scaler_mean)
    scale_arr = ", ".join(f"{x:.6f}f" for x in scaler_scale)
    f.write(f"static const float SCALER_MEAN[ML_FEATURE_COUNT] = {{ {mean_arr} }};\n")
    f.write(f"static const float SCALER_SCALE[ML_FEATURE_COUNT] = {{ {scale_arr} }};\n\n")
    
    # --- KMeans ---
    f.write(f"static const float KMEANS_ANOMALY_THRESHOLD = {kmeans_threshold:.6f}f;\n\n")
    
    # --- RandomForest ---
    f.write("// Mapeamento de classes do RandomForest\n")
    for i, class_name in enumerate(classes_mapping):
        f.write(f"// {i} = {class_name}\n")
    
print(f"Arquivo '{constants_h_path}' gerado com sucesso!")
print("\n EXPORTAÇÃO PARA C CONCLUÍDA.")
print("Próximo passo: rodar 'package_models.py' para juntar tudo.")