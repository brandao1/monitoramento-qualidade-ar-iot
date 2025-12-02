#pragma once

// Arquivo de Modelos de ML - Gerado Automaticamente

// Contém: Scaler, KMeans (Centroids/Threshold), RandomForest


#include <math.h> // Para powf, sqrtf


// --- Constantes (Scaler, Thresholds) --- 

// Constantes de Machine Learning exportadas

// Ordem das features: ['temperature', 'humidity', 'co2', 'co', 'tol', 'nh4', 'ace', 'o3']
static const int ML_FEATURE_COUNT = 14;

static const float SCALER_MEAN[ML_FEATURE_COUNT] = { 27.527869f, 59.598361f, 5606.674279f, 29.046091f, 6073.039168f, 5088.774807f, 3038.195672f, 3535.053590f, 109.560282f, 0.000000f, 239.753476f, 5.668033f, 10.061475f, 12.467213f };
static const float SCALER_SCALE[ML_FEATURE_COUNT] = { 0.195969f, 0.511745f, 803.673033f, 3.017173f, 976.158871f, 893.035560f, 372.203610f, 605.325060f, 5.810076f, 1.000000f, 13.611098f, 0.653210f, 0.844650f, 1.212533f };

static const float KMEANS_ANOMALY_THRESHOLD = 3.379581f;

// Mapeamento de classes do RandomForest
// 0 = ruim



// --- Centróides do KMeans --- 

static const int KMEANS_CLUSTERS = 3;
static const int KMEANS_FEATURES = 14;
static const float KMEANS_CENTROIDS[KMEANS_CLUSTERS][KMEANS_FEATURES] = {
    { -0.786261f, 0.646349f, -0.735525f, -0.478022f, -0.729517f, -0.724426f, -0.742718f, -0.725894f, -0.766465f, 0.000000f, -0.765203f, 0.151494f, 0.180094f, 0.199190f },
    { 0.787750f, -0.656077f, 0.084117f, -0.096287f, 0.076592f, 0.070344f, 0.093355f, 0.072133f, 0.125855f, 0.000000f, 0.124042f, -0.178550f, -0.216623f, -0.238874f },
    { -0.097186f, 0.106656f, 1.963488f, 1.751145f, 1.968970f, 1.973209f, 1.956205f, 1.972024f, 1.925865f, 0.000000f, 1.927748f, 0.102970f, 0.136145f, 0.148322f }
};



// --- Função de Classificação (RandomForest) --- 

float predict_rf(float * input) {
    return (1.0 + 1.0 + 1.0 + 1.0 + 1.0 + 1.0 + 1.0 + 1.0 + 1.0 + 1.0) * 0.1;
}


