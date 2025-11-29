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
      stmt: `SELECT 
        t, h, co2, co, tol, nh4, ace, o3, 
        no2, cl2, pm1_0, pm2_5, pm10_0, time_index
      FROM mttcc_service.etsensor 
      ORDER BY time_index DESC 
      LIMIT 1`,
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

  private mapRowToSensorData(cols: string[], row: (string | number | null)[]): SensorData {
    const data: Record<string, string | number | null> = {}

    cols.forEach((col, index) => {
      data[col] = row[index] ?? null
    })

    return {
      t: (data.t as number) ?? 0,
      h: (data.h as number) ?? 0,
      co2: (data.co2 as number) ?? 0,
      co: (data.co as number) ?? 0,
      tol: (data.tol as number) ?? 0,
      nh4: (data.nh4 as number) ?? 0,
      ace: (data.ace as number) ?? 0,
      o3: (data.o3 as number) ?? 0,
      no2: (data.no2 as number) ?? 0,
      cl2: (data.cl2 as number) ?? 0,
      pm1_0: (data.pm1_0 as number) ?? 0,
      pm2_5: (data.pm2_5 as number) ?? 0,
      pm10_0: (data.pm10_0 as number) ?? 0,
      time_index: (data.time_index as string) ?? new Date().toISOString(),
    }
  }
}

export const createAirQualityService = (
  crateDbHost: string = 'localhost',
  crateDbPort: number = 4200,
) => {
  return new AirQualityService(crateDbHost, crateDbPort)
}
