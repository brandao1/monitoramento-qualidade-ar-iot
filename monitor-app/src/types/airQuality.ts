export interface SensorData {
  t: number
  h: number
  co2: number
  co: number
  tol: number
  nh4: number
  ace: number
  o3: number
  no2: number
  cl2: number
  pm1_0: number
  pm2_5: number
  pm10_0: number
  qualidade_ar: string
  anomalia: boolean
  time_index: number
}

export interface CrateDBResponse {
  cols: string[]
  rows: (string | number | null)[][]
  rowCount: number
  duration: number
}

export interface SensorCard {
  key: keyof SensorData
  title: string
  value: string
  unit: string
  icon: string
  color: string
}

export interface AirQualityClassification {
  quality: string
  anomaly: boolean
  clusterDistance: number
}

export interface ApiConfig {
  crateDbHost: string
  crateDbPort: number
}
