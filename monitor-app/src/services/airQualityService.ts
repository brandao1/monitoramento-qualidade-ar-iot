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
     nitrogendioxide, chlorine, pm1, pm10, pm25, anomalia, qualidade_ar, time_index
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
        no2, cl2, pm1_0, pm2_5, pm10_0, anomalia, qualidade_ar, time_index
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

  async saveReferenceValues(references: any[]): Promise<void> {
    // Criar a tabela se não existir
    const createTableSql = {
      stmt: `CREATE TABLE IF NOT EXISTS "mttcc_service"."reference_values" (
        "metric_key" STRING PRIMARY KEY,
        "label" STRING,
        "unit" STRING,
        "icon" STRING,
        "min_value" DOUBLE,
        "max_value" DOUBLE,
        "updated_at" TIMESTAMP
      )`,
    }

    try {
      await fetch(this.crateDbUrl, {
        method: 'POST',
        headers: FIWARE_HEADERS,
        body: JSON.stringify(createTableSql),
      })

      // Inserir ou atualizar cada valor de referência
      for (const ref of references) {
        const upsertSql = {
          stmt: `INSERT INTO "mttcc_service"."reference_values" 
            (metric_key, label, unit, icon, min_value, max_value, updated_at) 
            VALUES (?, ?, ?, ?, ?, ?, ?) 
            ON CONFLICT (metric_key) DO UPDATE SET 
              label = excluded.label,
              unit = excluded.unit,
              icon = excluded.icon,
              min_value = excluded.min_value,
              max_value = excluded.max_value,
              updated_at = excluded.updated_at`,
          args: [ref.key, ref.label, ref.unit, ref.icon, ref.min, ref.max, Date.now()],
        }

        await fetch(this.crateDbUrl, {
          method: 'POST',
          headers: FIWARE_HEADERS,
          body: JSON.stringify(upsertSql),
        })
      }

      // Refresh para garantir que os dados foram salvos
      const refreshSql = {
        stmt: `REFRESH TABLE "mttcc_service"."reference_values"`,
      }

      await fetch(this.crateDbUrl, {
        method: 'POST',
        headers: FIWARE_HEADERS,
        body: JSON.stringify(refreshSql),
      })
    } catch (error) {
      console.error('Error saving reference values:', error)
      throw error
    }
  }

  async loadReferenceValues(): Promise<any[]> {
    const sql = {
      stmt: `SELECT metric_key, label, unit, icon, min_value, max_value 
             FROM "mttcc_service"."reference_values" 
             ORDER BY metric_key`,
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

      if (data.rowCount === 0) {
        return []
      }

      return data.rows.map((row) => ({
        key: row[0] as string,
        label: row[1] as string,
        unit: row[2] as string,
        icon: row[3] as string,
        min: Number(row[4]) || 0,
        max: Number(row[5]) || 0,
      }))
    } catch (error) {
      console.error('Error loading reference values:', error)
      return []
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
      qualidade_ar: 'qualidade_ar',
      anomalia: 'anomalia',
      
      time_index: 'time_index',
    }

    const data: Partial<SensorData> = {}

    cols.forEach((col, index) => {
      console.log('Mapping column:', col, 'with value:', row[index])
      const mappedKey = columnMapping[col]
      if (mappedKey) {
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
      qualidade_ar: String(data.qualidade_ar) || 'Desconhecida',
      anomalia: Boolean(data.anomalia) || false,
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
