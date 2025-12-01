<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { notificationService, type NotificationSettings } from '@/services/notificationService'

const settings = ref<NotificationSettings>({
  enabled: false,
  notifyAnomalies: true,
  notifyOutOfRange: true,
  soundEnabled: true,
})

const permissionStatus = ref<string>('default')
const successMessage = ref<string | null>(null)

function checkPermission() {
  if ('Notification' in window) {
    permissionStatus.value = Notification.permission
  } else {
    permissionStatus.value = 'not-supported'
  }
}

async function requestPermission() {
  const granted = await notificationService.requestPermission()
  checkPermission()
  
  if (granted) {
    settings.value.enabled = true
    saveSettings()
    showSuccess('Permissão concedida! Notificações ativadas.')
  } else {
    settings.value.enabled = false
    showSuccess('Permissão negada. Você pode alterar isso nas configurações do navegador.')
  }
}

function saveSettings() {
  notificationService.saveSettings(settings.value)
  showSuccess('Configurações salvas com sucesso!')
}

function showSuccess(message: string) {
  successMessage.value = message
  setTimeout(() => {
    successMessage.value = null
  }, 3000)
}

async function testNotification() {
  if (!settings.value.enabled) {
    alert('Ative as notificações primeiro!')
    return
  }

  await notificationService.notifyAnomaly('Esta é uma notificação de teste. O sistema está funcionando corretamente!')
}

function toggleNotifications() {
  if (settings.value.enabled && permissionStatus.value !== 'granted') {
    requestPermission()
  } else {
    saveSettings()
  }
}

onMounted(() => {
  checkPermission()
  settings.value = notificationService.getSettings()
})
</script>

<template>
  <div class="notification-settings">
    <div class="page-header">
      <h2><i class="fa-solid fa-bell"></i> Configurações de Notificações</h2>
      <p class="subtitle">Configure alertas para anomalias e valores fora do padrão</p>
    </div>

    <div v-if="successMessage" class="success-message">
      <i class="fa-solid fa-circle-check"></i> {{ successMessage }}
    </div>

    <div class="permission-section">
      <div class="permission-card">
        <div class="card-header">
          <div class="header-content">
            <i class="fa-solid fa-shield-halved"></i>
            <div>
              <h3>Permissão do Navegador</h3>
              <p class="permission-status" :class="permissionStatus">
                <i
                  :class="{
                    'fa-solid fa-circle-check': permissionStatus === 'granted',
                    'fa-solid fa-circle-xmark': permissionStatus === 'denied',
                    'fa-solid fa-circle-question': permissionStatus === 'default',
                    'fa-solid fa-ban': permissionStatus === 'not-supported',
                  }"
                ></i>
                {{
                  permissionStatus === 'granted'
                    ? 'Concedida'
                    : permissionStatus === 'denied'
                      ? 'Negada'
                      : permissionStatus === 'not-supported'
                        ? 'Não Suportado'
                        : 'Não Solicitada'
                }}
              </p>
            </div>
          </div>
          <button
            v-if="permissionStatus !== 'granted'"
            class="request-btn"
            @click="requestPermission"
            :disabled="permissionStatus === 'not-supported'"
          >
            <i class="fa-solid fa-hand-pointer"></i>
            Solicitar Permissão
          </button>
        </div>

        <div v-if="permissionStatus === 'denied'" class="warning-box">
          <i class="fa-solid fa-info-circle"></i>
          <p>
            As notificações foram bloqueadas. Para ativar, você precisa alterar as configurações do
            navegador manualmente.
          </p>
        </div>

        <div v-if="permissionStatus === 'not-supported'" class="warning-box error">
          <i class="fa-solid fa-triangle-exclamation"></i>
          <p>Seu navegador não suporta notificações web.</p>
        </div>
      </div>
    </div>

    <div class="settings-section">
      <div class="settings-card">
        <div class="card-header">
          <h3><i class="fa-solid fa-sliders"></i> Configurações de Alertas</h3>
          <button class="save-btn" @click="saveSettings">
            <i class="fa-solid fa-floppy-disk"></i> Salvar Configurações
          </button>
        </div>

        <div class="setting-item main-toggle">
          <div class="setting-info">
            <div class="setting-icon">
              <i class="fa-solid fa-bell"></i>
            </div>
            <div class="setting-details">
              <h4>Ativar Notificações</h4>
              <p>Habilita ou desabilita todas as notificações do sistema</p>
            </div>
          </div>
          <label class="toggle-switch">
            <input
              type="checkbox"
              v-model="settings.enabled"
              @change="toggleNotifications"
              :disabled="permissionStatus !== 'granted'"
            />
            <span class="slider"></span>
          </label>
        </div>

        <div class="divider"></div>

        <div class="setting-item" :class="{ disabled: !settings.enabled }">
          <div class="setting-info">
            <div class="setting-icon">
              <i class="fa-solid fa-triangle-exclamation"></i>
            </div>
            <div class="setting-details">
              <h4>Alertar Anomalias</h4>
              <p>Receber notificações quando anomalias forem detectadas nos sensores</p>
            </div>
          </div>
          <label class="toggle-switch">
            <input
              type="checkbox"
              v-model="settings.notifyAnomalies"
              @change="saveSettings"
              :disabled="!settings.enabled"
            />
            <span class="slider"></span>
          </label>
        </div>

        <div class="setting-item" :class="{ disabled: !settings.enabled }">
          <div class="setting-info">
            <div class="setting-icon">
              <i class="fa-solid fa-chart-line"></i>
            </div>
            <div class="setting-details">
              <h4>Alertar Valores Fora do Padrão</h4>
              <p>Receber notificações quando valores saírem dos limites de referência configurados</p>
            </div>
          </div>
          <label class="toggle-switch">
            <input
              type="checkbox"
              v-model="settings.notifyOutOfRange"
              @change="saveSettings"
              :disabled="!settings.enabled"
            />
            <span class="slider"></span>
          </label>
        </div>

        <div class="setting-item" :class="{ disabled: !settings.enabled }">
          <div class="setting-info">
            <div class="setting-icon">
              <i class="fa-solid fa-volume-high"></i>
            </div>
            <div class="setting-details">
              <h4>Som de Notificação</h4>
              <p>Tocar um som quando uma notificação for exibida</p>
            </div>
          </div>
          <label class="toggle-switch">
            <input
              type="checkbox"
              v-model="settings.soundEnabled"
              @change="saveSettings"
              :disabled="!settings.enabled"
            />
            <span class="slider"></span>
          </label>
        </div>
      </div>
    </div>

    <div class="test-section">
      <div class="test-card">
        <div class="test-content">
          <i class="fa-solid fa-flask"></i>
          <div>
            <h3>Testar Notificações</h3>
            <p>Envie uma notificação de teste para verificar se está funcionando corretamente</p>
          </div>
        </div>
        <button
          class="test-btn"
          @click="testNotification"
          :disabled="!settings.enabled || permissionStatus !== 'granted'"
        >
          <i class="fa-solid fa-paper-plane"></i>
          Enviar Teste
        </button>
      </div>
    </div>

    <div class="info-section">
      <div class="info-card">
        <i class="fa-solid fa-info-circle"></i>
        <h3>Como funciona?</h3>
        <ul>
          <li>
            <strong>Anomalias:</strong> Você será notificado quando o sistema detectar comportamentos
            anormais nos sensores.
          </li>
          <li>
            <strong>Valores Fora do Padrão:</strong> Receba alertas quando as medições ultrapassarem
            os limites configurados na página de Valores de Referência.
          </li>
          <li>
            <strong>Notificações em Tempo Real:</strong> As verificações acontecem automaticamente
            durante a atualização dos dados no Dashboard.
          </li>
          <li>
            <strong>Privacidade:</strong> Todas as configurações são salvas localmente no seu
            navegador.
          </li>
        </ul>
      </div>
    </div>
  </div>
</template>

<style scoped>
.notification-settings {
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

.success-message {
  background: #27ae60;
  color: white;
  padding: 16px;
  border-radius: 8px;
  text-align: center;
  margin-bottom: 20px;
  animation: slideIn 0.3s ease;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 8px;
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

.permission-section,
.settings-section,
.test-section,
.info-section {
  margin-bottom: 20px;
}

.permission-card,
.settings-card,
.test-card,
.info-card {
  background: rgba(0, 0, 0, 0.3);
  border: 1px solid rgba(255, 255, 255, 0.1);
  padding: 24px;
  border-radius: 12px;
}

.card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  flex-wrap: wrap;
  gap: 16px;
  margin-bottom: 20px;
}

.header-content {
  display: flex;
  align-items: center;
  gap: 16px;
}

.header-content > i {
  font-size: 2.5rem;
  color: #3498db;
}

.card-header h3 {
  margin: 0;
  font-size: 1.5rem;
}

.permission-status {
  display: flex;
  align-items: center;
  gap: 6px;
  font-size: 0.9rem;
  margin-top: 4px;
  opacity: 0.8;
}

.permission-status.granted {
  color: #27ae60;
}

.permission-status.denied {
  color: #e74c3c;
}

.permission-status.default {
  color: #f39c12;
}

.permission-status.not-supported {
  color: #e74c3c;
}

.request-btn,
.save-btn,
.test-btn {
  padding: 10px 20px;
  border: none;
  border-radius: 8px;
  font-size: 1rem;
  cursor: pointer;
  transition: all 0.3s;
  display: flex;
  align-items: center;
  gap: 8px;
  color: white;
}

.request-btn {
  background: #3498db;
}

.request-btn:hover:not(:disabled) {
  background: #2980b9;
  transform: translateY(-2px);
}

.save-btn {
  background: #27ae60;
}

.save-btn:hover {
  background: #219a52;
  transform: translateY(-2px);
}

.test-btn {
  background: #9b59b6;
}

.test-btn:hover:not(:disabled) {
  background: #8e44ad;
  transform: translateY(-2px);
}

button:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

.warning-box {
  background: rgba(241, 196, 15, 0.2);
  border: 1px solid rgba(241, 196, 15, 0.4);
  padding: 16px;
  border-radius: 8px;
  display: flex;
  align-items: flex-start;
  gap: 12px;
  margin-top: 16px;
}

.warning-box.error {
  background: rgba(231, 76, 60, 0.2);
  border-color: rgba(231, 76, 60, 0.4);
}

.warning-box i {
  font-size: 1.5rem;
  color: #f39c12;
  flex-shrink: 0;
}

.warning-box.error i {
  color: #e74c3c;
}

.warning-box p {
  margin: 0;
  line-height: 1.5;
}

.setting-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 20px 0;
  gap: 20px;
}

.setting-item.main-toggle {
  padding: 24px;
  background: rgba(52, 152, 219, 0.1);
  border: 1px solid rgba(52, 152, 219, 0.3);
  border-radius: 8px;
  margin-bottom: 20px;
}

.setting-item.disabled {
  opacity: 0.5;
}

.setting-info {
  display: flex;
  align-items: center;
  gap: 16px;
  flex: 1;
}

.setting-icon {
  width: 48px;
  height: 48px;
  background: rgba(255, 255, 255, 0.1);
  border-radius: 50%;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 1.5rem;
  flex-shrink: 0;
}

.setting-details h4 {
  margin: 0 0 4px 0;
  font-size: 1.1rem;
}

.setting-details p {
  margin: 0;
  opacity: 0.7;
  font-size: 0.9rem;
}

.toggle-switch {
  position: relative;
  display: inline-block;
  width: 60px;
  height: 34px;
  flex-shrink: 0;
}

.toggle-switch input {
  opacity: 0;
  width: 0;
  height: 0;
}

.slider {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: rgba(255, 255, 255, 0.2);
  transition: 0.4s;
  border-radius: 34px;
}

.slider:before {
  position: absolute;
  content: '';
  height: 26px;
  width: 26px;
  left: 4px;
  bottom: 4px;
  background-color: white;
  transition: 0.4s;
  border-radius: 50%;
}

input:checked + .slider {
  background-color: #27ae60;
}

input:checked + .slider:before {
  transform: translateX(26px);
}

input:disabled + .slider {
  cursor: not-allowed;
  opacity: 0.5;
}

.divider {
  height: 1px;
  background: rgba(255, 255, 255, 0.1);
  margin: 8px 0;
}

.test-content {
  display: flex;
  align-items: center;
  gap: 16px;
  flex: 1;
}

.test-content i {
  font-size: 2rem;
  color: #9b59b6;
}

.test-card {
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 20px;
  flex-wrap: wrap;
}

.test-content h3 {
  margin: 0 0 4px 0;
}

.test-content p {
  margin: 0;
  opacity: 0.7;
}

.info-card {
  background: rgba(52, 152, 219, 0.1);
  border-color: rgba(52, 152, 219, 0.3);
}

.info-card i {
  font-size: 1.5rem;
  color: #3498db;
  margin-bottom: 12px;
}

.info-card h3 {
  margin: 0 0 16px 0;
}

.info-card ul {
  margin: 0;
  padding-left: 20px;
}

.info-card li {
  margin-bottom: 12px;
  line-height: 1.6;
}

.info-card li:last-child {
  margin-bottom: 0;
}

@media (max-width: 768px) {
  .card-header,
  .test-card {
    flex-direction: column;
    align-items: stretch;
  }

  .setting-item {
    flex-direction: column;
    align-items: flex-start;
  }

  .toggle-switch {
    align-self: flex-end;
  }
}
</style>
