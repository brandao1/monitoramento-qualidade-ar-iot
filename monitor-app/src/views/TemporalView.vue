<script setup lang="ts">
import { ref, computed, onMounted } from 'vue'
import { createAirQualityService } from '@/services/airQualityService'
import type { SensorData } from '@/types/airQuality'

const crateDbHost = ref(import.meta.env.VITE_CRATEDB_HOST || 'localhost')
const crateDbPort = ref(Number(import.meta.env.VITE_CRATEDB_PORT) || 4200)

const startDate = ref('')
const endDate = ref('')
const sensorDataList = ref<SensorData[]>([])
const isLoading = ref(false)
const error = ref<string | null>(null)
const selectedMetric = ref('t')

const airQualityService = computed(() =>
  createAirQualityService(crateDbHost.value, crateDbPort.value),
)

const metrics = [
  { key: 't', label: 'Temperatura (°C)', icon: 'fa-solid fa-temperature-half' },
  { key: 'h', label: 'Umidade (%)', icon: 'fa-solid fa-droplet' },
  { key: 'co2', label: 'CO₂ (ppm)', icon: 'fa-solid fa-smog' },
  { key: 'co', label: 'CO (ppm)', icon: 'fa-solid fa-wind' },
  { key: 'tol', label: 'Tolueno (ppm)', icon: 'fa-solid fa-flask' },
  { key: 'nh4', label: 'Amônia (ppm)', icon: 'fa-solid fa-vial' },
  { key: 'ace', label: 'Acetona (ppm)', icon: 'fa-solid fa-microscope' },
  { key: 'o3', label: 'Ozônio (ppm)', icon: 'fa-solid fa-cloud' },
  { key: 'no2', label: 'NO₂ (ppm)', icon: 'fa-solid fa-industry' },
  { key: 'cl2', label: 'Cl₂ (ppm)', icon: 'fa-solid fa-triangle-exclamation' },
  { key: 'pm1_0', label: 'PM 1.0 (µg/m³)', icon: 'fa-solid fa-water' },
  { key: 'pm2_5', label: 'PM 2.5 (µg/m³)', icon: 'fa-solid fa-water' },
  { key: 'pm10_0', label: 'PM 10.0 (µg/m³)', icon: 'fa-solid fa-water' },
]

const chartData = computed(() => {
  if (!sensorDataList.value.length) return []
  
  return sensorDataList.value.map((data) => ({
    time: new Date(data.time_index).toLocaleString('pt-BR', {
      day: '2-digit',
      month: '2-digit',
      hour: '2-digit',
      minute: '2-digit',
    }),
    value: data[selectedMetric.value as keyof SensorData] as number,
  })).reverse() // Reverse to show oldest to newest
})

const statistics = computed(() => {
  if (!sensorDataList.value.length) return null

  const values = sensorDataList.value.map(
    (data) => data[selectedMetric.value as keyof SensorData] as number,
  )
  const sum = values.reduce((a, b) => a + b, 0)
  const avg = sum / values.length
  const min = Math.min(...values)
  const max = Math.max(...values)

  return {
    avg: avg.toFixed(2),
    min: min.toFixed(2),
    max: max.toFixed(2),
    count: values.length,
  }
})

const maxChartValue = computed(() => {
  if (!chartData.value.length) return 100
  const max = Math.max(...chartData.value.map((d) => d.value))
  return Math.ceil(max * 1.2) // 20% margin
})

function setDefaultDates() {
  const end = new Date()
  const start = new Date(end.getTime() - 24 * 60 * 60 * 1000) // 24 hours ago

  endDate.value = end.toISOString().slice(0, 16)
  startDate.value = start.toISOString().slice(0, 16)
}

async function fetchData() {
  if (!startDate.value || !endDate.value) {
    error.value = 'Por favor, selecione as datas de início e fim'
    return
  }

  isLoading.value = true
  error.value = null

  try {
    const data = await airQualityService.value.getHistoricalDataByDateRange(
      new Date(startDate.value),
      new Date(endDate.value),
    )
    sensorDataList.value = data
    
    if (data.length === 0) {
      error.value = 'Nenhum dado encontrado para o período selecionado'
    }
  } catch (err) {
    error.value = err instanceof Error ? err.message : 'Erro ao carregar dados'
    console.error('Error fetching data:', err)
  } finally {
    isLoading.value = false
  }
}

function getChartBarHeight(value: number): number {
  return (value / maxChartValue.value) * 100
}

function getChartBarColor(value: number): string {
  const metric = metrics.find((m) => m.key === selectedMetric.value)
  if (!metric) return '#3498db'

  // Color coding based on metric type
  if (selectedMetric.value === 't') {
    if (value < 18) return '#3498db'
    if (value < 26) return '#27ae60'
    if (value < 32) return '#f39c12'
    return '#e74c3c'
  } else if (selectedMetric.value === 'co2') {
    if (value < 800) return '#27ae60'
    if (value < 1200) return '#f39c12'
    return '#e74c3c'
  } else if (selectedMetric.value === 'pm2_5') {
    if (value < 12) return '#27ae60'
    if (value < 35) return '#f39c12'
    return '#e74c3c'
  }

  return '#3498db'
}

onMounted(() => {
  setDefaultDates()
})
</script>

<template>
  <div class="temporal-view">
    <div class="filters-section">
      <div class="date-filters">
        <div class="date-group">
          <label for="startDate">Data Inicial:</label>
          <input id="startDate" v-model="startDate" type="datetime-local" />
        </div>
        <div class="date-group">
          <label for="endDate">Data Final:</label>
          <input id="endDate" v-model="endDate" type="datetime-local" />
        </div>
        <button class="fetch-btn" @click="fetchData" :disabled="isLoading">
          <i :class="isLoading ? 'fa-solid fa-hourglass-half' : 'fa-solid fa-chart-bar'"></i>
          {{ isLoading ? ' Carregando...' : ' Buscar Dados' }}
        </button>
      </div>

      <div class="metric-selector">
        <label for="metric">Métrica:</label>
        <select id="metric" v-model="selectedMetric">
          <option v-for="metric in metrics" :key="metric.key" :value="metric.key">
            {{ metric.label }}
          </option>
        </select>
      </div>
    </div>

    <div v-if="error" class="error-message"><i class="fa-solid fa-triangle-exclamation"></i> {{ error }}</div>

    <div v-if="isLoading" class="loading">
      <div class="loading-spinner"></div>
      <p>Carregando dados históricos...</p>
    </div>

    <div v-else-if="sensorDataList.length > 0" class="results-section">
      <div class="statistics-cards">
        <div class="stat-card">
          <div class="stat-label">Média</div>
          <div class="stat-value">{{ statistics?.avg }}</div>
        </div>
        <div class="stat-card">
          <div class="stat-label">Mínimo</div>
          <div class="stat-value">{{ statistics?.min }}</div>
        </div>
        <div class="stat-card">
          <div class="stat-label">Máximo</div>
          <div class="stat-value">{{ statistics?.max }}</div>
        </div>
        <div class="stat-card">
          <div class="stat-label">Registros</div>
          <div class="stat-value">{{ statistics?.count }}</div>
        </div>
      </div>

      <div class="chart-container">
        <h3>
          <i :class="metrics.find((m) => m.key === selectedMetric)?.icon"></i>
          {{ metrics.find((m) => m.key === selectedMetric)?.label }}
        </h3>
        <div class="chart">
          <div class="chart-bars">
            <div
              v-for="(point, index) in chartData"
              :key="index"
              class="chart-bar-wrapper"
              :title="`${point.time}: ${point.value.toFixed(2)}`"
            >
              <div
                class="chart-bar"
                :data-value="point.value.toFixed(2)"
                :style="{
                  height: `${getChartBarHeight(point.value)}%`,
                  backgroundColor: getChartBarColor(point.value),
                }"
              ></div>
              <div v-if="chartData.length <= 20" class="chart-label">
                {{ point.time }}
              </div>
            </div>
          </div>
        </div>
      </div>

      <div class="data-table">
        <h3><i class="fa-solid fa-clipboard-list"></i> Dados Detalhados</h3>
        <div class="table-wrapper">
          <table>
            <thead>
              <tr>
                <th>Data/Hora</th>
                <th>Valor</th>
              </tr>
            </thead>
            <tbody>
              <tr v-for="(data, index) in sensorDataList" :key="index">
                <td>{{ new Date(data.time_index).toLocaleString('pt-BR') }}</td>
                <td>{{ Number(data[selectedMetric]).toFixed(2) }}</td>
              </tr>
            </tbody>
          </table>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.temporal-view {
  width: 100%;
  color: white;
}

.filters-section {
  background: rgba(0, 0, 0, 0.3);
  border: 1px solid rgba(255, 255, 255, 0.1);
  padding: 20px;
  border-radius: 12px;
  margin-bottom: 20px;
}

.date-filters {
  display: flex;
  gap: 16px;
  justify-content: center;
  align-items: flex-end;
  flex-wrap: wrap;
  margin-bottom: 16px;
}

.date-group {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.date-group label {
  font-size: 0.875rem;
}

.date-group input {
  padding: 10px 14px;
  border: 1px solid rgba(255, 255, 255, 0.2);
  border-radius: 8px;
  font-size: 1rem;
  background: rgba(30, 30, 46, 0.8);
  color: white;
}

.date-group input:focus {
  outline: none;
  border-color: #3498db;
}

.date-group input::-webkit-calendar-picker-indicator {
  filter: invert(1);
}

.fetch-btn {
  padding: 10px 24px;
  background: #27ae60;
  color: white;
  border: none;
  border-radius: 8px;
  font-size: 1rem;
  cursor: pointer;
  transition: background 0.3s;
}

.fetch-btn:hover:not(:disabled) {
  background: #219a52;
}

.fetch-btn:disabled {
  opacity: 0.6;
  cursor: not-allowed;
}

.metric-selector {
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 10px;
}

.metric-selector select {
  padding: 10px 14px;
  border: 1px solid rgba(255, 255, 255, 0.2);
  border-radius: 8px;
  font-size: 1rem;
  background: rgba(30, 30, 46, 0.8);
  color: white;
  cursor: pointer;
  min-width: 250px;
}

.metric-selector select:focus {
  outline: none;
  border-color: #3498db;
}

.error-message {
  background: #e74c3c;
  color: white;
  padding: 16px;
  border-radius: 8px;
  text-align: center;
  margin-bottom: 20px;
}

.loading {
  text-align: center;
  padding: 40px;
}

.loading-spinner {
  width: 50px;
  height: 50px;
  border: 4px solid rgba(255, 255, 255, 0.3);
  border-top-color: white;
  border-radius: 50%;
  animation: spin 1s linear infinite;
  margin: 0 auto 20px;
}

@keyframes spin {
  to {
    transform: rotate(360deg);
  }
}

.results-section {
  display: flex;
  flex-direction: column;
  gap: 20px;
}

.statistics-cards {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
  gap: 16px;
}

.stat-card {
  background: rgba(0, 0, 0, 0.3);
  border: 1px solid rgba(255, 255, 255, 0.1);
  padding: 20px;
  border-radius: 12px;
  text-align: center;
}

.stat-label {
  font-size: 0.875rem;
  opacity: 0.8;
  margin-bottom: 8px;
}

.stat-value {
  font-size: 2rem;
  font-weight: bold;
}

.chart-container {
  background: rgba(0, 0, 0, 0.3);
  border: 1px solid rgba(255, 255, 255, 0.1);
  padding: 20px;
  border-radius: 12px;
}

.chart-container h3 {
  margin-bottom: 20px;
  text-align: center;
}

.chart {
  height: 400px;
  position: relative;
  background: rgba(0, 0, 0, 0.4);
  border-radius: 8px;
  padding: 20px;
  overflow-x: auto;
  overflow-y: hidden;
}

.chart-bars {
  display: flex;
  align-items: flex-end;
  justify-content: flex-start;
  height: 100%;
  gap: 4px;
  min-width: fit-content;
}

.chart-bar-wrapper {
  flex: 0 0 auto;
  display: flex;
  flex-direction: column;
  align-items: center;
  height: 100%;
  min-width: 30px;
  width: 40px;
}

.chart-bar {
  width: 100%;
  min-height: 2px;
  border-radius: 4px 4px 0 0;
  transition: all 0.3s;
  cursor: pointer;
  position: relative;
}

.chart-bar:hover {
  opacity: 0.8;
  transform: scaleY(1.05);
}

.chart-bar::after {
  content: attr(data-value);
  position: absolute;
  bottom: -30px;
  left: 50%;
  transform: translateX(-50%);
  background: rgba(0, 0, 0, 0.9);
  color: white;
  padding: 4px 8px;
  border-radius: 4px;
  font-size: 0.75rem;
  white-space: nowrap;
  opacity: 0;
  pointer-events: none;
  transition: opacity 0.2s;
  z-index: 10;
}

.chart-bar:hover::after {
  opacity: 1;
}

.chart-label {
  font-size: 0.7rem;
  margin-top: 8px;
  writing-mode: vertical-rl;
  text-orientation: mixed;
  white-space: nowrap;
  opacity: 0.7;
}

.data-table {
  background: rgba(0, 0, 0, 0.3);
  border: 1px solid rgba(255, 255, 255, 0.1);
  padding: 20px;
  border-radius: 12px;
}

.data-table h3 {
  margin-bottom: 16px;
}

.table-wrapper {
  max-height: 400px;
  overflow-y: auto;
  border-radius: 8px;
}

table {
  width: 100%;
  border-collapse: collapse;
  background: rgba(0, 0, 0, 0.4);
}

thead {
  position: sticky;
  top: 0;
  background: rgba(0, 0, 0, 0.7);
  z-index: 1;
}

th,
td {
  padding: 12px;
  text-align: left;
  border-bottom: 1px solid rgba(255, 255, 255, 0.1);
}

th {
  font-weight: bold;
}

tbody tr:hover {
  background: rgba(255, 255, 255, 0.1);
}

@media (max-width: 600px) {
  .date-filters {
    flex-direction: column;
    align-items: stretch;
  }

  .date-group input,
  .metric-selector select {
    width: 100%;
  }

  .chart {
    height: 300px;
  }

  .chart-bar-wrapper {
    min-width: 25px;
    width: 30px;
  }

  .chart-label {
    font-size: 0.6rem;
  }

  .statistics-cards {
    grid-template-columns: repeat(2, 1fr);
  }
}
</style>
