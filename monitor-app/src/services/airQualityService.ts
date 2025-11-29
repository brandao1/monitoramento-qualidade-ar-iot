import type { CrateDBResponse, SensorData } from '@/types/airQuality'

const FIWARE_HEADERS = {
  'Content-Type': 'application/json',
  'fiware-service': 'mttcc_service',
  'fiware-servicepath': '/',
}

export class AirQualityService {
  private crateDbUrl: string

  constructor(crateDbHost: string = 'localhost', crateDbPort: number = 4200) {
    this.crateDbUrl = `http://${crateDbHost}:${crateDbPort}/_sql`
  }

  async getLatestSensorData(): Promise<SensorData | null> {
    const sql = {
      stmt: `SELECT  temperature, humidity, carbonmonoxide, carbondioxide, toluene, ammonium, acetone, ozone,
     nitrogendioxide, chlorine, pm1, pm10, pm25, qualidade_ar, time_index
      FROM "mttcc_service"."etsensor"
      ORDER BY time_index DESC
      LIMIT 1;`,
    }


    try {
      const response = await fetch(this.crateDbUrl, {
        method: 'POST',
        headers: FIWARE_HEADERS,
        body: JSON.stringify(sql),
      })

      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`)
      }

      const data: CrateDBResponse = await response.json()

      if (data.rowCount === 0 || !data.rows[0]) {
        return null
      }
      console.log('tatatata:', this.mapRowToSensorData(data.cols, data.rows[0]))
      return this.mapRowToSensorData(data.cols, data.rows[0])
    } catch (error) {
      console.error('Error fetching latest sensor data:', error)
      throw error
    }
  }

  async getHistoricalData(hours: number = 24): Promise<SensorData[]> {
    const sql = {
      stmt: `SELECT 
        t, h, co2, co, tol, nh4, ace, o3, 
        no2, cl2, pm1_0, pm2_5, pm10_0, time_index
      FROM mttcc_service.etsensor 
      WHERE time_index >= NOW() - INTERVAL '${hours} hours'
      ORDER BY time_index DESC 
      LIMIT 100`,
    }

    try {
      const response = await fetch(this.crateDbUrl, {
        method: 'POST',
        headers: FIWARE_HEADERS,
        body: JSON.stringify(sql),
      })

      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`)
      }

      const data: CrateDBResponse = await response.json()

      return data.rows.map((row) => this.mapRowToSensorData(data.cols, row))
    } catch (error) {
      console.error('Error fetching historical data:', error)
      throw error
    }
  }

  async getHistoricalDataByDateRange(startDate: Date, endDate: Date): Promise<SensorData[]> {
    const startTimestamp = startDate.getTime()
    const endTimestamp = endDate.getTime()

    const sql = {
      stmt: `SELECT 
        temperature, humidity, carbonmonoxide, carbondioxide, toluene, ammonium, 
        acetone, ozone, nitrogendioxide, chlorine, pm1, pm25, pm10, time_index
      FROM "mttcc_service"."etsensor" 
      WHERE time_index >= ${startTimestamp} AND time_index <= ${endTimestamp}
      ORDER BY time_index DESC 
      LIMIT 1000`,
    }

    try {
      const response = await fetch(this.crateDbUrl, {
        method: 'POST',
        headers: FIWARE_HEADERS,
        body: JSON.stringify(sql),
      })

      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`)
      }

      const data: CrateDBResponse = await response.json()

      return data.rows.map((row) => this.mapRowToSensorData(data.cols, row))
    } catch (error) {
      console.error('Error fetching historical data by date range:', error)
      throw error
    }
  }

  private mapRowToSensorData(cols: string[], row: (string | number | null)[]): SensorData {
    const columnMapping: Record<string, keyof SensorData> = {
      temperature: 't',
      humidity: 'h',
      carbondioxide: 'co2',
      carbonmonoxide: 'co',
      toluene: 'tol',
      ammonium: 'nh4',
      acetone: 'ace',
      ozone: 'o3',
      nitrogendioxide: 'no2',
      chlorine: 'cl2',
      pm1: 'pm1_0',
      pm25: 'pm2_5',
      pm10: 'pm10_0',
      time_index: 'time_index',
    }

    const data: Partial<SensorData> = {}

    cols.forEach((col, index) => {
      console.log('Mapping column:', col, 'with value:', row[index])
      const mappedKey = columnMapping[col]
      if (mappedKey) {
        // O CrateDB retorna o timestamp como um número grande (milissegundos)
        data[mappedKey] = row[index] as any
      }
    })

    return {
      t: Number(data.t) || 0,
      h: Number(data.h) || 0,
      co2: Number(data.co2) || 0,
      co: Number(data.co) || 0,
      tol: Number(data.tol) || 0,
      nh4: Number(data.nh4) || 0,
      ace: Number(data.ace) || 0,
      o3: Number(data.o3) || 0,
      no2: Number(data.no2) || 0,
      cl2: Number(data.cl2) || 0,
      pm1_0: Number(data.pm1_0) || 0,
      pm2_5: Number(data.pm2_5) || 0,
      pm10_0: Number(data.pm10_0) || 0,
      time_index: Number(data.time_index) || 0,
    }
  }
}

export const createAirQualityService = (
  crateDbHost: string = 'localhost',
  crateDbPort: number = 4200,
) => {
  return new AirQualityService(crateDbHost, crateDbPort)
}
