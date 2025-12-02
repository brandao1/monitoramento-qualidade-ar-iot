export interface NotificationSettings {
  enabled: boolean
  notifyAnomalies: boolean
  notifyOutOfRange: boolean
  soundEnabled: boolean
}

export class NotificationService {
  private settings: NotificationSettings = {
    enabled: false,
    notifyAnomalies: true,
    notifyOutOfRange: true,
    soundEnabled: true,
  }

  private notificationSound: HTMLAudioElement | null = null
  private permissionGranted: boolean = false

  constructor() {
    this.loadSettings()
    this.initSound()
  }

  private loadSettings(): void {
    const saved = localStorage.getItem('notificationSettings')
    if (saved) {
      try {
        this.settings = JSON.parse(saved)
      } catch (err) {
        console.error('Error loading notification settings:', err)
      }
    }
  }

  saveSettings(settings: NotificationSettings): void {
    this.settings = settings
    localStorage.setItem('notificationSettings', JSON.stringify(settings))
  }

  getSettings(): NotificationSettings {
    return { ...this.settings }
  }

  private initSound(): void {
    try {
      const audioContext = new (window.AudioContext || (window as any).webkitAudioContext)()
      
      this.notificationSound = new Audio()
      const oscillator = audioContext.createOscillator()
      const gainNode = audioContext.createGain()
      
      oscillator.connect(gainNode)
      gainNode.connect(audioContext.destination)
      
      oscillator.frequency.value = 800
      gainNode.gain.value = 0.3
    } catch (err) {
      console.warn('Audio context not available:', err)
    }
  }

  async requestPermission(): Promise<boolean> {
    if (!('Notification' in window)) {
      console.warn('This browser does not support notifications')
      return false
    }

    if (Notification.permission === 'granted') {
      this.permissionGranted = true
      return true
    }

    if (Notification.permission !== 'denied') {
      const permission = await Notification.requestPermission()
      this.permissionGranted = permission === 'granted'
      return this.permissionGranted
    }

    return false
  }

  private playSound(): void {
    if (!this.settings.soundEnabled) return

    try {
      const audioContext = new (window.AudioContext || (window as any).webkitAudioContext)()
      const oscillator = audioContext.createOscillator()
      const gainNode = audioContext.createGain()

      oscillator.connect(gainNode)
      gainNode.connect(audioContext.destination)

      oscillator.frequency.value = 800
      oscillator.type = 'sine'
      gainNode.gain.value = 0.3

      oscillator.start(audioContext.currentTime)
      oscillator.stop(audioContext.currentTime + 0.2)
    } catch (err) {
      console.warn('Could not play notification sound:', err)
    }
  }

  async notifyAnomaly(message: string): Promise<void> {
    if (!this.settings.enabled || !this.settings.notifyAnomalies) return

    this.playSound()

    if (this.permissionGranted && Notification.permission === 'granted') {
      new Notification('⚠️ Anomalia Detectada', {
        body: message,
        icon: '/favicon.ico',
        badge: '/favicon.ico',
        tag: 'anomaly',
        requireInteraction: true,
      })
    }
  }

  async notifyOutOfRange(metric: string, currentValue: number, min: number, max: number, unit: string): Promise<void> {
    if (!this.settings.enabled || !this.settings.notifyOutOfRange) return

    this.playSound()

    const status = currentValue < min ? 'abaixo' : 'acima'
    const reference = currentValue < min ? min : max
    const message = `${metric}: ${currentValue.toFixed(2)} ${unit} está ${status} do limite (${reference} ${unit})`

    if (this.permissionGranted && Notification.permission === 'granted') {
      new Notification('📊 Valor Fora do Padrão', {
        body: message,
        icon: '/favicon.ico',
        badge: '/favicon.ico',
        tag: `out-of-range-${metric}`,
        requireInteraction: false,
      })
    }
  }

  isEnabled(): boolean {
    return this.settings.enabled
  }
}

export const notificationService = new NotificationService()
