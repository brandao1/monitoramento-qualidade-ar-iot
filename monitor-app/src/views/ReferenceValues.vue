<script setup lang="ts">
import { ref, onMounted, computed } from 'vue'
import { createAirQualityService } from '@/services/airQualityService'
import type { SensorData } from '@/types/airQuality'

interface ReferenceValue {
  key: string
  label: string
  unit: string
  icon: string
  min: number
  max: number
}

const crateDbHost = ref(import.meta.env.VITE_CRATEDB_HOST || 'localhost')
const crateDbPort = ref(Number(import.meta.env.VITE_CRATEDB_PORT) || 4200)

const referenceValues = ref<ReferenceValue[]>([
  { key: 't', label: 'Temperatura', unit: '°C', icon: 'fa-solid fa-temperature-half', min: 18, max: 26 },
  { key: 'h', label: 'Umidade', unit: '%', icon: 'fa-solid fa-droplet', min: 40, max: 60 },
  { key: 'co2', label: 'CO₂', unit: 'ppm', icon: 'fa-solid fa-smog', min: 0, max: 1000 },
  { key: 'co', label: 'CO', unit: 'ppm', icon: 'fa-solid fa-wind', min: 0, max: 9 },
  { key: 'tol', label: 'Tolueno', unit: 'ppm', icon: 'fa-solid fa-flask', min: 0, max: 1 },
  { key: 'nh4', label: 'Amônia', unit: 'ppm', icon: 'fa-solid fa-vial', min: 0, max: 25 },
  { key: 'ace', label: 'Acetona', unit: 'ppm', icon: 'fa-solid fa-microscope', min: 0, max: 1 },
  { key: 'o3', label: 'Ozônio', unit: 'ppm', icon: 'fa-solid fa-cloud', min: 0, max: 0.1 },
  { key: 'no2', label: 'NO₂', unit: 'ppm', icon: 'fa-solid fa-industry', min: 0, max: 0.05 },
  { key: 'cl2', label: 'Cl₂', unit: 'ppm', icon: 'fa-solid fa-triangle-exclamation', min: 0, max: 0.5 },
  { key: 'pm1_0', label: 'PM 1.0', unit: 'µg/m³', icon: 'fa-solid fa-water', min: 0, max: 25 },
  { key: 'pm2_5', label: 'PM 2.5', unit: 'µg/m³', icon: 'fa-solid fa-water', min: 0, max: 12 },
  { key: 'pm10_0', label: 'PM 10.0', unit: 'µg/m³', icon: 'fa-solid fa-water', min: 0, max: 54 },
])

const sensorData = ref<SensorData | null>(null)
const isLoading = ref(false)
const isLoadingReferences = ref(true)
const isSaving = ref(false)
const error = ref<string | null>(null)
const successMessage = ref<string | null>(null)

const airQualityService = computed(() =>
  createAirQualityService(crateDbHost.value, crateDbPort.value),
)

const comparison = computed(() => {
  if (!sensorData.value) return null

  return referenceValues.value.map((ref) => {
    const currentValue = sensorData.value![ref.key as keyof SensorData] as number
    const isWithinRange = currentValue >= ref.min && currentValue <= ref.max
    const status = isWithinRange ? 'normal' : currentValue < ref.min ? 'below' : 'above'
    
    return {
      ...ref,
      currentValue: currentValue.toFixed(2),
      isWithinRange,
      status,
      difference: currentValue - ref.max,
    }
  })
})

async function loadLatestData() {
  isLoading.value = true
  error.value = null

  try {
    const data = await airQualityService.value.getLatestSensorData()
    sensorData.value = data
  } catch (err) {
    error.value = err instanceof Error ? err.message : 'Erro ao carregar dados'
    console.error('Error fetching data:', err)
  } finally {
    isLoading.value = false
  }
}

async function saveReferences() {
  isSaving.value = true
  error.value = null
  successMessage.value = null

  try {
    await airQualityService.value.saveReferenceValues(referenceValues.value)
    
    successMessage.value = 'Valores de referência salvos no CrateDB com sucesso!'
    setTimeout(() => {
      successMessage.value = null
    }, 3000)
  } catch (err) {
    error.value = err instanceof Error ? err.message : 'Erro ao salvar valores de referência no CrateDB'
    console.error('Error saving references:', err)
  } finally {
    isSaving.value = false
  }
}

async function loadReferences() {
  isLoadingReferences.value = true
  try {
    const saved = await airQualityService.value.loadReferenceValues()
    if (saved && saved.length > 0) {
      referenceValues.value = saved
    }
  } catch (err) {
    console.error('Error loading saved references from CrateDB:', err)
    // Se falhar ao carregar do CrateDB, mantém os valores padrão
  } finally {
    isLoadingReferences.value = false
  }
}

function resetDefaults() {
  referenceValues.value = [
    { key: 't', label: 'Temperatura', unit: '°C', icon: 'fa-solid fa-temperature-half', min: 18, max: 26 },
    { key: 'h', label: 'Umidade', unit: '%', icon: 'fa-solid fa-droplet', min: 40, max: 60 },
    { key: 'co2', label: 'CO₂', unit: 'ppm', icon: 'fa-solid fa-smog', min: 0, max: 1000 },
    { key: 'co', label: 'CO', unit: 'ppm', icon: 'fa-solid fa-wind', min: 0, max: 9 },
    { key: 'tol', label: 'Tolueno', unit: 'ppm', icon: 'fa-solid fa-flask', min: 0, max: 1 },
    { key: 'nh4', label: 'Amônia', unit: 'ppm', icon: 'fa-solid fa-vial', min: 0, max: 25 },
    { key: 'ace', label: 'Acetona', unit: 'ppm', icon: 'fa-solid fa-microscope', min: 0, max: 1 },
    { key: 'o3', label: 'Ozônio', unit: 'ppm', icon: 'fa-solid fa-cloud', min: 0, max: 0.1 },
    { key: 'no2', label: 'NO₂', unit: 'ppm', icon: 'fa-solid fa-industry', min: 0, max: 0.05 },
    { key: 'cl2', label: 'Cl₂', unit: 'ppm', icon: 'fa-solid fa-triangle-exclamation', min: 0, max: 0.5 },
    { key: 'pm1_0', label: 'PM 1.0', unit: 'µg/m³', icon: 'fa-solid fa-water', min: 0, max: 25 },
    { key: 'pm2_5', label: 'PM 2.5', unit: 'µg/m³', icon: 'fa-solid fa-water', min: 0, max: 12 },
    { key: 'pm10_0', label: 'PM 10.0', unit: 'µg/m³', icon: 'fa-solid fa-water', min: 0, max: 54 },
  ]
}

onMounted(async () => {
  await loadReferences()
})
</script>

<template>
  <div class="reference-values">
    <div class="page-header">
      <h2><i class="fa-solid fa-sliders"></i> Valores de Referência</h2>
      <p class="subtitle">Configure os limites aceitáveis para cada métrica de qualidade do ar</p>
    </div>

    <div class="config-section">
      <div class="config-group">
        <label for="crateHost">Host CrateDB:</label>
        <input id="crateHost" v-model="crateDbHost" type="text" placeholder="localhost" />
      </div>
      <div class="config-group">
        <label for="cratePort">Porta:</label>
        <input id="cratePort" v-model.number="crateDbPort" type="number" placeholder="4200" />
      </div>
    </div>

    <div v-if="error" class="error-message">
      <i class="fa-solid fa-triangle-exclamation"></i> {{ error }}
    </div>

    <div v-if="successMessage" class="success-message">
      <i class="fa-solid fa-circle-check"></i> {{ successMessage }}
    </div>

    <div class="reference-section">
      <div class="section-header">
        <h3><i class="fa-solid fa-pen-to-square"></i> Editar Valores de Referência</h3>
        <div class="action-buttons">
          <button class="reset-btn" @click="resetDefaults" :disabled="isLoadingReferences">
            <i class="fa-solid fa-rotate-left"></i> Resetar Padrões
          </button>
          <button class="save-btn" @click="saveReferences" :disabled="isSaving || isLoadingReferences">
            <i :class="isSaving ? 'fa-solid fa-spinner fa-spin' : 'fa-solid fa-floppy-disk'"></i>
            {{ isSaving ? ' Salvando...' : ' Salvar no CrateDB' }}
          </button>
        </div>
      </div>

      <div v-if="isLoadingReferences" class="loading-references">
        <i class="fa-solid fa-spinner fa-spin"></i>
        <p>Carregando valores de referência do CrateDB...</p>
      </div>

      <div v-else class="reference-grid">
        <div v-for="ref in referenceValues" :key="ref.key" class="reference-card">
          <div class="card-header">
            <i :class="ref.icon"></i>
            <span class="label">{{ ref.label }}</span>
          </div>
          <div class="input-group">
            <div class="input-field">
              <label>Mínimo:</label>
              <div class="input-with-unit">
                <input v-model.number="ref.min" type="number" step="0.01" />
                <span class="unit">{{ ref.unit }}</span>
              </div>
            </div>
            <div class="input-field">
              <label>Máximo:</label>
              <div class="input-with-unit">
                <input v-model.number="ref.max" type="number" step="0.01" />
                <span class="unit">{{ ref.unit }}</span>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>

    <div class="comparison-section">
      <div class="section-header">
        <h3><i class="fa-solid fa-code-compare"></i> Comparação com Valores Atuais</h3>
        <button class="load-btn" @click="loadLatestData" :disabled="isLoading">
          <i :class="isLoading ? 'fa-solid fa-spinner fa-spin' : 'fa-solid fa-arrows-rotate'"></i>
          {{ isLoading ? ' Carregando...' : ' Carregar Dados Atuais' }}
        </button>
      </div>

      <div v-if="comparison && sensorData" class="comparison-grid">
        <div
          v-for="item in comparison"
          :key="item.key"
          class="comparison-card"
          :class="item.status"
        >
          <div class="comparison-header">
            <i :class="item.icon"></i>
            <span class="label">{{ item.label }}</span>
            <span class="status-badge" :class="item.status">
              <i
                :class="
                  item.isWithinRange
                    ? 'fa-solid fa-circle-check'
                    : 'fa-solid fa-circle-exclamation'
                "
              ></i>
              {{ item.isWithinRange ? 'Normal' : item.status === 'below' ? 'Abaixo' : 'Acima' }}
            </span>
          </div>
          <div class="comparison-values">
            <div class="value-item">
              <span class="value-label">Atual:</span>
              <span class="value-current" :class="{ warning: !item.isWithinRange }">
                {{ item.currentValue }} {{ item.unit }}
              </span>
            </div>
            <div class="value-item">
              <span class="value-label">Referência:</span>
              <span class="value-ref">{{ item.min }} - {{ item.max }} {{ item.unit }}</span>
            </div>
          </div>
          <div v-if="!item.isWithinRange" class="warning-info">
            <i class="fa-solid fa-info-circle"></i>
            <span v-if="item.status === 'above'">
              {{ Math.abs(item.difference).toFixed(2) }} {{ item.unit }} acima do limite
            </span>
            <span v-else>
              {{ Math.abs(Number(item.currentValue) - item.min).toFixed(2) }} {{ item.unit }} abaixo
              do mínimo
            </span>
          </div>
        </div>
      </div>

      <div v-else-if="!sensorData" class="no-data">
        <i class="fa-solid fa-inbox"></i>
        <p>Clique em "Carregar Dados Atuais" para ver a comparação</p>
      </div>
    </div>
  </div>
</template>

<style scoped>
.reference-values {
  width: 100%;
  color: white;
}

.page-header {
  text-align: center;
  margin-bottom: 30px;
}

.page-header h2 {
  font-size: 2rem;
  margin-bottom: 8px;
}

.subtitle {
  opacity: 0.8;
  font-size: 1rem;
}

.config-section {
  display: flex;
  gap: 16px;
  justify-content: center;
  align-items: flex-end;
  margin-bottom: 20px;
  flex-wrap: wrap;
  background: rgba(0, 0, 0, 0.3);
  padding: 16px;
  border-radius: 12px;
  border: 1px solid rgba(255, 255, 255, 0.1);
}

.config-group {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.config-group label {
  font-size: 0.875rem;
}

.config-group input {
  padding: 10px 14px;
  border: 1px solid rgba(255, 255, 255, 0.2);
  border-radius: 8px;
  font-size: 1rem;
  background: rgba(30, 30, 46, 0.8);
  color: white;
  min-width: 150px;
}

.config-group input:focus {
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

.success-message {
  background: #27ae60;
  color: white;
  padding: 16px;
  border-radius: 8px;
  text-align: center;
  margin-bottom: 20px;
  animation: slideIn 0.3s ease;
}

@keyframes slideIn {
  from {
    opacity: 0;
    transform: translateY(-10px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

.reference-section,
.comparison-section {
  background: rgba(0, 0, 0, 0.3);
  border: 1px solid rgba(255, 255, 255, 0.1);
  padding: 20px;
  border-radius: 12px;
  margin-bottom: 20px;
}

.loading-references {
  text-align: center;
  padding: 60px 20px;
  color: rgba(255, 255, 255, 0.8);
}

.loading-references i {
  font-size: 3rem;
  margin-bottom: 16px;
  display: block;
}

.section-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
  flex-wrap: wrap;
  gap: 16px;
}

.section-header h3 {
  font-size: 1.5rem;
  display: flex;
  align-items: center;
  gap: 10px;
}

.action-buttons {
  display: flex;
  gap: 10px;
}

.save-btn,
.reset-btn,
.load-btn {
  padding: 10px 20px;
  border: none;
  border-radius: 8px;
  font-size: 1rem;
  cursor: pointer;
  transition: all 0.3s;
  display: flex;
  align-items: center;
  gap: 6px;
}

.save-btn {
  background: #27ae60;
  color: white;
}

.save-btn:hover:not(:disabled) {
  background: #219a52;
  transform: translateY(-2px);
}

.save-btn:disabled {
  opacity: 0.6;
  cursor: not-allowed;
}

.reset-btn {
  background: rgba(231, 76, 60, 0.3);
  color: white;
  border: 1px solid rgba(231, 76, 60, 0.5);
}

.reset-btn:hover {
  background: rgba(231, 76, 60, 0.5);
  transform: translateY(-2px);
}

.load-btn {
  background: #3498db;
  color: white;
}

.load-btn:hover:not(:disabled) {
  background: #2980b9;
  transform: translateY(-2px);
}

.load-btn:disabled {
  opacity: 0.6;
  cursor: not-allowed;
}

.reference-grid,
.comparison-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(280px, 1fr));
  gap: 16px;
}

.reference-card {
  background: rgba(30, 30, 46, 0.8);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 12px;
  padding: 16px;
}

.reference-card .card-header {
  display: flex;
  align-items: center;
  gap: 8px;
  margin-bottom: 16px;
  font-size: 1.1rem;
  font-weight: 600;
}

.reference-card .card-header i {
  font-size: 1.3rem;
}

.input-group {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.input-field label {
  font-size: 0.875rem;
  opacity: 0.8;
  margin-bottom: 4px;
  display: block;
}

.input-with-unit {
  display: flex;
  align-items: center;
  gap: 8px;
  background: rgba(0, 0, 0, 0.3);
  border: 1px solid rgba(255, 255, 255, 0.2);
  border-radius: 8px;
  padding: 8px 12px;
}

.input-with-unit input {
  flex: 1;
  background: none;
  border: none;
  color: white;
  font-size: 1rem;
}

.input-with-unit input:focus {
  outline: none;
}

.input-with-unit .unit {
  opacity: 0.6;
  font-size: 0.875rem;
}

.comparison-card {
  background: rgba(30, 30, 46, 0.8);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 12px;
  padding: 16px;
  transition: all 0.3s;
}

.comparison-card.above {
  border-color: rgba(231, 76, 60, 0.5);
}

.comparison-card.below {
  border-color: rgba(241, 196, 15, 0.5);
}

.comparison-card.normal {
  border-color: rgba(39, 174, 96, 0.5);
}

.comparison-header {
  display: flex;
  align-items: center;
  gap: 8px;
  margin-bottom: 12px;
  flex-wrap: wrap;
}

.comparison-header i {
  font-size: 1.3rem;
}

.comparison-header .label {
  flex: 1;
  font-weight: 600;
  font-size: 1.1rem;
}

.status-badge {
  padding: 4px 12px;
  border-radius: 12px;
  font-size: 0.85rem;
  display: flex;
  align-items: center;
  gap: 4px;
}

.status-badge.normal {
  background: rgba(39, 174, 96, 0.3);
  color: #27ae60;
}

.status-badge.above,
.status-badge.below {
  background: rgba(231, 76, 60, 0.3);
  color: #e74c3c;
}

.comparison-values {
  display: flex;
  flex-direction: column;
  gap: 8px;
  margin-bottom: 12px;
}

.value-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.value-label {
  opacity: 0.7;
  font-size: 0.9rem;
}

.value-current {
  font-weight: bold;
  font-size: 1.1rem;
}

.value-current.warning {
  color: #e74c3c;
}

.value-ref {
  opacity: 0.8;
}

.warning-info {
  background: rgba(231, 76, 60, 0.2);
  padding: 8px;
  border-radius: 6px;
  font-size: 0.85rem;
  display: flex;
  align-items: center;
  gap: 6px;
  color: #e74c3c;
}

.no-data {
  text-align: center;
  padding: 60px 20px;
  opacity: 0.6;
}

.no-data i {
  font-size: 3rem;
  margin-bottom: 16px;
  display: block;
}

@media (max-width: 600px) {
  .section-header {
    flex-direction: column;
    align-items: stretch;
  }

  .action-buttons {
    flex-direction: column;
  }

  .reference-grid,
  .comparison-grid {
    grid-template-columns: 1fr;
  }
}
</style>
