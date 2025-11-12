// Constantes de Machine Learning exportadas

// Ordem das features: ['temperature', 'humidity', 'co2', 'co', 'tol', 'nh4', 'ace', 'o3']
static const int ML_FEATURE_COUNT = 14;

static const float SCALER_MEAN[ML_FEATURE_COUNT] = { 27.527869f, 59.598361f, 5606.674279f, 29.046091f, 6073.039168f, 5088.774807f, 3038.195672f, 3535.053590f, 109.560282f, 0.000000f, 239.753476f, 5.668033f, 10.061475f, 12.467213f };
static const float SCALER_SCALE[ML_FEATURE_COUNT] = { 0.195969f, 0.511745f, 803.673033f, 3.017173f, 976.158871f, 893.035560f, 372.203610f, 605.325060f, 5.810076f, 1.000000f, 13.611098f, 0.653210f, 0.844650f, 1.212533f };

static const float KMEANS_ANOMALY_THRESHOLD = 3.379581f;

// Mapeamento de classes do RandomForest
// 0 = ruim
