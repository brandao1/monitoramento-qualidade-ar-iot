import re
import os

MODEL_DIR = "modelos_treinados"
FINAL_HEADER_FILE = "ml_models.h"

# Arquivos de entrada gerados pelo Passo 2
rf_c_file = os.path.join(MODEL_DIR, "random_forest_model.c")
kmeans_h_file = os.path.join(MODEL_DIR, "kmeans_centroids.h")
constants_h_file = os.path.join(MODEL_DIR, "ml_constants.h")

print(f"Iniciando empacotamento para '{FINAL_HEADER_FILE}'...")

try:
    with open(rf_c_file, "r") as f:
        rf_code = f.read()
    with open(kmeans_h_file, "r") as f:
        kmeans_code = f.read()
    with open(constants_h_file, "r") as f:
        constants_code = f.read()
except FileNotFoundError as e:
    print(f"Erro: Arquivo não encontrado. {e}")
    print("Você executou o script 'export_models_to_c.py' (Passo 2) primeiro?")
    exit(1)

# --- Otimizar código C do m2cgen para ESP32 ---

# 1. Renomear a função de 'score' para algo único
# m2cgen gera uma função 'score'
rf_code = rf_code.replace("double score(", "float predict_rf(")

# 2. Trocar 'double' por 'float' (mais leve para o ESP32)
rf_code = rf_code.replace("double", "float")

# 3. Trocar funções de math para suas versões float
rf_code = rf_code.replace("pow(", "powf(")
rf_code = rf_code.replace("sqrt(", "sqrtf(")
rf_code = rf_code.replace("fabs(", "fabsf(")

# 4. Remover includes desnecessários (serão incluídos no .ino ou .h principal)
rf_code = re.sub(r'#include\s+<math.h>\s*', '', rf_code)
rf_code = re.sub(r'#include\s+<string.h>\s*', '', rf_code)

# --- Montar o arquivo final ---

final_header_content = []
final_header_content.append("#pragma once\n")
final_header_content.append("// Arquivo de Modelos de ML - Gerado Automaticamente\n")
final_header_content.append("// Contém: Scaler, KMeans (Centroids/Threshold), RandomForest\n\n")

final_header_content.append("#include <math.h> // Para powf, sqrtf\n\n")

final_header_content.append("// --- Constantes (Scaler, Thresholds) --- \n")
final_header_content.append(constants_code)
final_header_content.append("\n")

final_header_content.append("// --- Centróides do KMeans --- \n")
final_header_content.append(kmeans_code)
final_header_content.append("\n")

final_header_content.append("// --- Função de Classificação (RandomForest) --- \n")
final_header_content.append(rf_code)
final_header_content.append("\n")

with open(FINAL_HEADER_FILE, "w") as f:
    f.write("\n".join(final_header_content))

print("="*50)
print(f" SUCESSO! Arquivo '{FINAL_HEADER_FILE}' foi gerado.")
print("Próximo passo: Copie o conteúdo deste arquivo para a aba 'ml_models.h' no seu projeto Arduino/ESP32.")
print("="*50)