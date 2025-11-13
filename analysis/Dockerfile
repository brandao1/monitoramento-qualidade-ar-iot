# 1. Usar uma imagem base leve do Python
FROM python:3.9-slim

# 2. Definir o diretório de trabalho dentro do contêiner
WORKDIR /app

# 3. Copiar os arquivos necessários para dentro do contêiner
# Copia o script da aplicação
COPY app.py .
# Copia a pasta com os modelos treinados
COPY modelos_treinados/ ./modelos_treinados/

# 4. Instalar as dependências Python
RUN pip install scikit-learn==1.0.2 pandas==1.3.5 joblib==1.1.0 numpy==1.21.6 flask==2.0.2
# Nota: É crucial usar versões compatíveis com as usadas no treinamento.
# Se você usou versões diferentes, ajuste esta linha.

# 5. Comando para executar o script quando o contêiner iniciar
CMD ["python", "-u", "app.py"]