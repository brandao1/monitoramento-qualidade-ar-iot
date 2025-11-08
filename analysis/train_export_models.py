import joblib
import numpy as np
import m2cgen as m2c
import os

MODEL_DIR = "modelos_treinados"

# --------- CARREGAR MODELOS TREINADOS ----------
kmeans_path = os.path.join(MODEL_DIR, "kmeans_model.pkl")
rf_path = os.path.join(MODEL_DIR, "random_forest_model.pkl")

kmeans = joblib.load(kmeans_path)
rf = joblib.load(rf_path)

# --------- EXPORTAR RANDOM FOREST PARA C ---------
print("Gerando código C do RandomForest...")
rf_c_code = m2c.export_to_c(rf)

with open(os.path.join(MODEL_DIR, "random_forest_model.c"), "w") as f:
    f.write(rf_c_code)

print("Arquivo 'random_forest_model.c' gerado com sucesso!")


# --------- EXPORTAR CENTROIDES DO KMEANS ---------
print("Gerando centroides do KMeans...")

centroids = kmeans.cluster_centers_
n_clusters, n_features = centroids.shape

with open(os.path.join(MODEL_DIR, "kmeans_centroids.h"), "w") as f:
    f.write("#pragma once\n")
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

print("Arquivo 'kmeans_centroids.h' gerado com sucesso!")
