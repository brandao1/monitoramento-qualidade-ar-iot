# train_export_models.py
# Requisitos: pip install scikit-learn joblib pandas numpy

import os
import json
import numpy as np
import pandas as pd
from crate import client
from sklearn.preprocessing import StandardScaler
from sklearn.cluster import KMeans
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score
import joblib

# Configuração
OUTPUT_DIR = "modelos_treinados"
os.makedirs(OUTPUT_DIR, exist_ok=True)

# --- Conexão CrateDB ---
try:
    connection = client.connect("http://localhost:4200")
    print("Conexão com CrateDB bem-sucedida!")
except Exception as e:
    print(f"Erro ao conectar ao CrateDB: {e}")
    raise SystemExit(1)

sql_query = """
    SELECT 
        time_index, 
        temperature, 
        humidity, 
        carbondioxide, 
        carbonmonoxide, 
        alcohol, 
        toluene, 
        ammonium, 
        acetone,
        ozone,
        nitrogendioxide,
        chlorine,
        pm1,
        pm25,
        pm10 
    FROM "mttcc_service"."etsensor"
    ORDER BY time_index DESC
"""

try:
    df = pd.read_sql(sql_query, connection)
    print(f"Dados carregados com sucesso! {len(df)} linhas encontradas.")
    numeric_cols = ['temperature', 'humidity', 'carbondioxide', 'carbonmonoxide', 'alcohol', 'toluene', 'ammonium', 'acetone', 'ozone', 'nitrogendioxide', 'chlorine', 'pm1', 'pm25', 'pm10']
    df[numeric_cols] = df[numeric_cols].apply(pd.to_numeric, errors='coerce')
except Exception as e:
    print(f"Erro ao buscar dados: {e}")
    connection.close()
    raise SystemExit(1)

connection.close()

if df.empty:
    print("DataFrame está vazio. Nada a fazer.")
    raise SystemExit(0)

# --- Limpeza ---
features = ['temperature', 'humidity', 'carbondioxide', 'carbonmonoxide', 'alcohol', 'toluene', 'ammonium', 'acetone', 'ozone', 'nitrogendioxide', 'chlorine', 'pm1', 'pm25', 'pm10']
df_clean = df.dropna(subset=features)
print(f"Removidas {len(df) - len(df_clean)} linhas com dados ausentes.")

if len(df_clean) < 20:
    print("Dados insuficientes após limpeza.")
    raise SystemExit(1)

X = df_clean[features].copy()

# ---------- SCALER ----------
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

# ---------- KMEANS ----------
print("Treinando KMeans...")
n_clusters = 3
kmeans = KMeans(n_clusters=n_clusters, random_state=42, n_init=10)
kmeans.fit(X_scaled)

distances = kmeans.transform(X_scaled).min(axis=1)
threshold = float(np.mean(distances) * 1.5)
is_anomaly = distances > threshold
df_clean["is_anomaly"] = is_anomaly

print(f"KMeans treinado. {is_anomaly.sum()} anomalias detectadas.")

# Converter centroides para escala original
centroids_scaled = kmeans.cluster_centers_
centroids_orig = scaler.inverse_transform(centroids_scaled).tolist()

with open(os.path.join(OUTPUT_DIR, "centroids.json"), "w") as f:
    json.dump(
        {
            "centroids": centroids_orig,
            "threshold": threshold,
            "features": features
        },
        f,
        indent=2
    )

joblib.dump(kmeans, os.path.join(OUTPUT_DIR, "kmeans_model.pkl"))
joblib.dump(scaler, os.path.join(OUTPUT_DIR, "scaler.pkl"))

# ---------- RANDOM FOREST ----------
print("Gerando labels de qualidade...")

def classificar_qualidade_ar(row):
    if row['ozone'] > 50 or row['carbonmonoxide'] > 500 or row['carbondioxide'] > 2000:
        return 'ruim'
    elif row['ozone'] > 30 or row['carbonmonoxide'] > 200 or row['carbondioxide'] > 1000:
        return 'moderada'
    else:
        return 'boa'

y = df_clean.apply(classificar_qualidade_ar, axis=1)

# if len(y.unique()) < 2:
#     raise SystemExit("Erro: Somente uma classe encontrada.")

X_train, X_test, y_train, y_test = train_test_split(
    X_scaled, y, test_size=0.3, random_state=42, stratify=y
)

rf_model = RandomForestClassifier(
    n_estimators=10,
    max_depth=6,
    random_state=42
)

rf_model.fit(X_train, y_train)
y_pred = rf_model.predict(X_test)

acc = accuracy_score(y_test, y_pred)
print(f"Acurácia RandomForest: {acc*100:.2f}%")

joblib.dump(rf_model, os.path.join(OUTPUT_DIR, "random_forest_model.pkl"))

print("\n MODELOS TREINADOS COM SUCESSO")
print("Arquivos salvos em:", OUTPUT_DIR)
print("\nPróximo passo: rodar exportação para C via m2cgen:")
print("python exportar_para_c.py")
