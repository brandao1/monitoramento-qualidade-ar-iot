FROM python:3.9-slim

WORKDIR /app


COPY app.py .

COPY modelos_treinados/ ./modelos_treinados/


RUN pip install scikit-learn==1.0.2 pandas==1.3.5 joblib==1.1.0 numpy==1.21.6 flask==2.0.2

CMD ["python", "-u", "app.py"]