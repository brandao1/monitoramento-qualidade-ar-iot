<script setup lang="ts">
import { RouterView, RouterLink } from 'vue-router'
</script>

<template>
  <div class="app-container">
    <header class="app-header">
      <h1><i class="fa-solid fa-wind"></i> Monitor de Qualidade do Ar</h1>
      <p class="subtitle">Sistema de Monitoramento IoT em Tempo Real</p>
      
      <nav class="nav-menu">
        <RouterLink to="/" class="nav-link"><i class="fa-solid fa-gauge"></i> Dashboard</RouterLink>
        <RouterLink to="/temporal" class="nav-link"><i class="fa-solid fa-chart-line"></i> Visualização Temporal</RouterLink>
        <RouterLink to="/reference" class="nav-link"><i class="fa-solid fa-sliders"></i> Valores de Referência</RouterLink>
      </nav>
    </header>

    <main class="main-content">
      <RouterView />
    </main>
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
  background: linear-gradient(135deg, #1a1a2e 0%, #16213e 50%, #0f3460 100%);
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
  margin-bottom: 20px;
}

.nav-menu {
  display: flex;
  justify-content: center;
  gap: 16px;
  flex-wrap: wrap;
  margin-top: 20px;
}

.nav-link {
  padding: 12px 24px;
  background: rgba(255, 255, 255, 0.1);
  color: white;
  text-decoration: none;
  border-radius: 8px;
  font-size: 1rem;
  transition: all 0.3s;
  border: 2px solid rgba(255, 255, 255, 0.2);
}

.nav-link:hover {
  background: rgba(255, 255, 255, 0.2);
  transform: translateY(-2px);
  border-color: rgba(255, 255, 255, 0.4);
}

.nav-link.router-link-active {
  background: rgba(52, 152, 219, 0.3);
  border-color: #3498db;
  font-weight: bold;
}

.main-content {
  width: 100%;
}

@media (max-width: 600px) {
  .app-header h1 {
    font-size: 1.8rem;
  }

  .nav-menu {
    flex-direction: column;
    align-items: stretch;
  }

  .nav-link {
    text-align: center;
  }
}
</style>
