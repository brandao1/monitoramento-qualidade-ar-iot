export interface SensorData {
  temperature: number
  humidity: number
  co2: number
  co: number
  tolueno: number
  amonia: number
  acetona: number
  ozonio: number
  no2: number
  cl2: number
  pm1_0: number
  pm2_5: number
  pm10_0: number
  time_index: string
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
