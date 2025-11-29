<script setup lang="ts">
defineProps<{
  title: string
  value: string
  unit: string
  icon: string
  color: string
  visible: boolean
}>()

defineEmits<{
  (e: 'toggle'): void
}>()
</script>

<template>
  <div class="sensor-card" :class="{ collapsed: !visible }">
    <div class="card-header">
      <div class="card-title">
        <span class="icon"><i :class="icon"></i></span>
        <h3>{{ title }}</h3>
      </div>
      <button class="toggle-btn" @click="$emit('toggle')" :title="visible ? 'Ocultar' : 'Mostrar'">
        <i :class="visible ? 'fa-solid fa-eye' : 'fa-solid fa-eye-slash'"></i>
      </button>
    </div>
    <div v-if="visible" class="card-content">
      <div class="value-container">
        <span class="value" :style="{ color: color }">{{ value }}</span>
        <span class="unit">{{ unit }}</span>
      </div>
    </div>
  </div>
</template>

<style scoped>
.sensor-card {
  background: rgba(30, 30, 46, 0.8);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 12px;
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.3);
  padding: 16px;
  transition: all 0.3s ease;
  min-width: 200px;
}

.sensor-card:hover {
  transform: translateY(-2px);
  box-shadow: 0 6px 16px rgba(0, 0, 0, 0.4);
  border-color: rgba(255, 255, 255, 0.2);
}

.sensor-card.collapsed {
  opacity: 0.7;
}

.card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 12px;
}

.card-title {
  display: flex;
  align-items: center;
  gap: 8px;
}

.card-title h3 {
  margin: 0;
  font-size: 1rem;
  color: #e0e0e0;
  font-weight: 600;
}

.icon {
  font-size: 1.5rem;
  display: flex;
  align-items: center;
  justify-content: center;
  width: 1.5rem;
}

.toggle-btn {
  background: none;
  border: none;
  cursor: pointer;
  font-size: 1.2rem;
  color: rgba(255, 255, 255, 0.6);
  opacity: 0.6;
  transition: all 0.2s;
}

.toggle-btn:hover {
  opacity: 1;
  color: rgba(255, 255, 255, 0.9);
}

.card-content {
  animation: fadeIn 0.3s ease;
}

@keyframes fadeIn {
  from {
    opacity: 0;
    transform: translateY(-10px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

.value-container {
  display: flex;
  align-items: baseline;
  gap: 8px;
}

.value {
  font-size: 2rem;
  font-weight: 700;
}

.unit {
  font-size: 1rem;
  color: #666;
}
</style>
