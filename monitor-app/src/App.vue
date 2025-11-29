<script setup lang="ts">
import { ref, onMounted, onUnmounted, computed } from 'vue'
import SensorCard from '@/components/SensorCard.vue'
import { createAirQualityService } from '@/services/airQualityService'
import type { SensorData, SensorCard as SensorCardType } from '@/types/airQuality'

const sensorData = ref<SensorData | null>(null)
const isLoading = ref(false)
const error = ref<string | null>(null)
const lastUpdate = ref<string>('')
const autoRefreshInterval = ref<number>(-1)
const cardVisibility = ref<Record<string, boolean>>({})

const crateDbHost = ref(import.meta.env.VITE_CRATEDB_HOST || 'localhost')
const crateDbPort = ref(Number(import.meta.env.VITE_CRATEDB_PORT) || 4200)

let refreshTimer: ReturnType<typeof setInterval> | null = null

const airQualityService = computed(() =>
  createAirQualityService(crateDbHost.value, crateDbPort.value),
)

const sensorCards = computed<SensorCardType[]>(() => {
  if (!sensorData.value) return []

  return [
    {
      key: 't',
      title: 'Temperatura',
      value: sensorData.value.t.toFixed(1),
      unit: '°C',
      icon: '🌡️',
      color: getTemperatureColor(sensorData.value.t),
    },
    {
      key: 'h',
      title: 'Umidade',
      value: sensorData.value.h.toFixed(1),
      unit: '%',
      icon: '💧',
      color: '#3498db',
    },
    {
      key: 'co2',
      title: 'CO₂',
      value: sensorData.value.co2.toFixed(0),
      unit: 'ppm',
      icon: '🌫️',
      color: getCO2Color(sensorData.value.co2),
    },
    {
      key: 'co',
      title: 'CO',
      value: sensorData.value.co.toFixed(2),
      unit: 'ppm',
      icon: '💨',
      color: getCOColor(sensorData.value.co),
    },
    {
      key: 'tol',
      title: 'Tolueno',
      value: sensorData.value.tol.toFixed(2),
      unit: 'ppm',
      icon: '🧪',
      color: '#9b59b6',
    },
    {
      key: 'nh4',
      title: 'Amônia (NH₄)',
      value: sensorData.value.nh4.toFixed(2),
      unit: 'ppm',
      icon: '⚗️',
      color: '#e67e22',
    },
    {
      key: 'ace',
      title: 'Acetona',
      value: sensorData.value.ace.toFixed(2),
      unit: 'ppm',
      icon: '🔬',
      color: '#1abc9c',
    },
    {
      key: 'o3',
      title: 'Ozônio (O₃)',
      value: sensorData.value.o3.toFixed(3),
      unit: 'ppm',
      icon: '☁️',
      color: '#3498db',
    },
    {
      key: 'no2',
      title: 'NO₂',
      value: sensorData.value.no2.toFixed(3),
      unit: 'ppm',
      icon: '🏭',
      color: '#e74c3c',
    },
    {
      key: 'cl2',
      title: 'Cl₂',
      value: sensorData.value.cl2.toFixed(3),
      unit: 'ppm',
      icon: '⚠️',
      color: '#f1c40f',
    },
    {
      key: 'pm1_0',
      title: 'PM 1.0',
      value: sensorData.value.pm1_0.toFixed(0),
      unit: 'µg/m³',
      icon: '🌁',
      color: getPMColor(sensorData.value.pm1_0),
    },
    {
      key: 'pm2_5',
      title: 'PM 2.5',
      value: sensorData.value.pm2_5.toFixed(0),
      unit: 'µg/m³',
      icon: '🌁',
      color: getPM25Color(sensorData.value.pm2_5),
    },
    {
      key: 'pm10_0',
      title: 'PM 10.0',
      value: sensorData.value.pm10_0.toFixed(0),
      unit: 'µg/m³',
      icon: '🌁',
      color: getPM10Color(sensorData.value.pm10_0),
    },
  ]
})

function getTemperatureColor(temp: number): string {
  if (temp < 18) return '#3498db'
  if (temp < 26) return '#27ae60'
  if (temp < 32) return '#f39c12'
  return '#e74c3c'
}

function getCO2Color(co2: number): string {
  if (co2 < 800) return '#27ae60'
  if (co2 < 1200) return '#f39c12'
  return '#e74c3c'
}

function getCOColor(co: number): string {
  if (co < 9) return '#27ae60'
  if (co < 35) return '#f39c12'
  return '#e74c3c'
}

function getPMColor(pm: number): string {
  if (pm < 25) return '#27ae60'
  if (pm < 50) return '#f39c12'
  return '#e74c3c'
}

function getPM25Color(pm: number): string {
  if (pm < 12) return '#27ae60'
  if (pm < 35) return '#f39c12'
  return '#e74c3c'
}

function getPM10Color(pm: number): string {
  if (pm < 54) return '#27ae60'
  if (pm < 154) return '#f39c12'
  return '#e74c3c'
}

async function fetchData() {
  isLoading.value = true
  error.value = null

  try {
    const data = await airQualityService.value.getLatestSensorData()
    sensorData.value = data
    lastUpdate.value = new Date().toLocaleString('pt-BR')
  } catch (err) {
    error.value = err instanceof Error ? err.message : 'Erro ao carregar dados'
    console.error('Error fetching data:', err)
  } finally {
    isLoading.value = false
  }
}

function toggleCardVisibility(key: string) {
  cardVisibility.value[key] = !cardVisibility.value[key]
}

function isCardVisible(key: string): boolean {
  if (cardVisibility.value[key] === undefined) {
    cardVisibility.value[key] = true
  }
  return cardVisibility.value[key]
}

function handleAutoRefreshChange(event: Event) {
  const target = event.target as HTMLSelectElement
  const newInterval = Number(target.value)
  autoRefreshInterval.value = newInterval

  if (refreshTimer) {
    clearInterval(refreshTimer)
    refreshTimer = null
  }

  if (newInterval > 0) {
    refreshTimer = setInterval(fetchData, newInterval)
  }
}

function updateConfig() {
  fetchData()
}

onMounted(() => {
  sensorCards.value.forEach((card) => {
    cardVisibility.value[card.key] = true
  })
  fetchData()
})

onUnmounted(() => {
  if (refreshTimer) {
    clearInterval(refreshTimer)
  }
})
</script>

<template>
  <div class="app-container">
    <header class="app-header">
      <h1>🌬️ Monitor de Qualidade do Ar</h1>
      <p class="subtitle">Sistema de Monitoramento IoT em Tempo Real</p>
    </header>

    <div class="config-section">
      <div class="config-group">
        <label for="crateHost">Host CrateDB:</label>
        <input id="crateHost" v-model="crateDbHost" type="text" placeholder="localhost" />
      </div>
      <div class="config-group">
        <label for="cratePort">Porta:</label>
        <input id="cratePort" v-model.number="crateDbPort" type="number" placeholder="4200" />
      </div>
      <button class="config-btn" @click="updateConfig">Conectar</button>
    </div>

    <div class="controls-section">
      <div class="auto-refresh">
        <label for="autoRefresh">Atualização automática:</label>
        <select id="autoRefresh" @change="handleAutoRefreshChange">
          <option value="-1">Desabilitada</option>
          <option value="2000">A cada 2 segundos</option>
          <option value="5000">A cada 5 segundos</option>
          <option value="10000">A cada 10 segundos</option>
          <option value="30000">A cada 30 segundos</option>
          <option value="60000">A cada 1 minuto</option>
        </select>
      </div>
      <button class="refresh-btn" @click="fetchData" :disabled="isLoading">
        {{ isLoading ? '⏳ Carregando...' : '🔄 Atualizar Agora' }}
      </button>
    </div>

    <div v-if="lastUpdate" class="last-update">Última atualização: {{ lastUpdate }}</div>

    <div v-if="error" class="error-message">⚠️ {{ error }}</div>

    <div v-if="isLoading && !sensorData" class="loading">
      <div class="loading-spinner"></div>
      <p>Carregando dados dos sensores...</p>
    </div>

    <div v-else-if="!sensorData && !error" class="no-data">
      <p>📊 Nenhum dado disponível. Configure a conexão e clique em "Conectar".</p>
    </div>

    <div v-else-if="sensorData" class="sensor-grid">
      <SensorCard
        v-for="card in sensorCards"
        :key="card.key"
        :title="card.title"
        :value="card.value"
        :unit="card.unit"
        :icon="card.icon"
        :color="card.color"
        :visible="isCardVisible(card.key)"
        @toggle="toggleCardVisibility(card.key)"
      />
    </div>

    <div v-if="sensorData" class="timestamp-info">
      <p>
        📅 Dados coletados em:
        {{ new Date(sensorData.time_index).toLocaleString('pt-BR') }}
      </p>
    </div>
  </div>
</template>

<style>
* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
}

body {
  font-family:
    -apple-system,
    BlinkMacSystemFont,
    'Segoe UI',
    Roboto,
    Oxygen,
    Ubuntu,
    sans-serif;
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  min-height: 100vh;
}

.app-container {
  max-width: 1400px;
  margin: 0 auto;
  padding: 20px;
}

.app-header {
  text-align: center;
  color: white;
  margin-bottom: 30px;
}

.app-header h1 {
  font-size: 2.5rem;
  margin-bottom: 8px;
  text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.2);
}

.subtitle {
  font-size: 1.1rem;
  opacity: 0.9;
}

.config-section {
  display: flex;
  gap: 16px;
  justify-content: center;
  align-items: flex-end;
  margin-bottom: 20px;
  flex-wrap: wrap;
  background: rgba(255, 255, 255, 0.1);
  padding: 16px;
  border-radius: 12px;
}

.config-group {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.config-group label {
  color: white;
  font-size: 0.875rem;
}

.config-group input {
  padding: 10px 14px;
  border: none;
  border-radius: 8px;
  font-size: 1rem;
  background: white;
  min-width: 150px;
}

.config-btn {
  padding: 10px 24px;
  background: #27ae60;
  color: white;
  border: none;
  border-radius: 8px;
  font-size: 1rem;
  cursor: pointer;
  transition: background 0.3s;
}

.config-btn:hover {
  background: #219a52;
}

.controls-section {
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 20px;
  margin-bottom: 20px;
  flex-wrap: wrap;
}

.auto-refresh {
  display: flex;
  align-items: center;
  gap: 10px;
  color: white;
}

.auto-refresh select {
  padding: 10px 14px;
  border: none;
  border-radius: 8px;
  font-size: 1rem;
  cursor: pointer;
  background: white;
}

.refresh-btn {
  padding: 12px 24px;
  background: #3498db;
  color: white;
  border: none;
  border-radius: 8px;
  font-size: 1rem;
  cursor: pointer;
  transition: all 0.3s;
}

.refresh-btn:hover:not(:disabled) {
  background: #2980b9;
  transform: scale(1.02);
}

.refresh-btn:disabled {
  opacity: 0.6;
  cursor: not-allowed;
}

.last-update {
  text-align: center;
  color: rgba(255, 255, 255, 0.8);
  margin-bottom: 20px;
  font-size: 0.9rem;
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
  color: white;
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

.no-data {
  text-align: center;
  color: white;
  padding: 40px;
  background: rgba(255, 255, 255, 0.1);
  border-radius: 12px;
}

.sensor-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(220px, 1fr));
  gap: 20px;
  margin-bottom: 20px;
}

.timestamp-info {
  text-align: center;
  color: rgba(255, 255, 255, 0.8);
  font-size: 0.9rem;
  padding: 10px;
}

@media (max-width: 600px) {
  .app-header h1 {
    font-size: 1.8rem;
  }

  .config-section {
    flex-direction: column;
    align-items: stretch;
  }

  .config-group input {
    width: 100%;
  }

  .sensor-grid {
    grid-template-columns: 1fr;
  }
}
</style>
