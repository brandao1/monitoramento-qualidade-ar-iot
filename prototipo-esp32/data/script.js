document.addEventListener('DOMContentLoaded', function() {
    const SENSOR_CONFIG = {
        temperature: { label: 'Temperatura', unit: '°C', icon: 'fas fa-thermometer-half' },
        humidity: { label: 'Umidade', unit: '%', icon: 'fas fa-tint' },
        pm1_0: { label: 'PM1.0', unit: 'µg/m³', icon: 'fas fa-atom' },
        pm2_5: { label: 'PM2.5', unit: 'µg/m³', icon: 'fas fa-atom' },
        pm10_0: { label: 'PM10', unit: 'µg/m³', icon: 'fas fa-atom' },
        co2: { label: 'CO₂', unit: 'ppb', icon: 'fas fa-smog' },
        co: { label: 'CO', unit: 'ppb', icon: 'fas fa-burn' },
        toluene: { label: 'Tolueno', unit: 'ppb', icon: 'fas fa-flask' },
        nh4: { label: 'Amônia (NH₄)', unit: 'ppb', icon: 'fas fa-vial' },
        acetone: { label: 'Acetona', unit: 'ppb', icon: 'fas fa-spray-can' },
        o3: { label: 'Ozônio (O₃)', unit: 'ppb', icon: 'fas fa-cloud' },
        no2: { label: 'Dióxido de Nitrogênio (NO₂)', unit: 'ppb', icon: 'fas fa-cloud' },
        cl2: { label: 'Cloro (Cl₂)', unit: 'ppb', icon: 'fas fa-cloud' },
        alcohol: { label: 'Álcool', unit: 'ppb', icon: 'fas fa-vial' }
    };

    function updateStatus() {
        fetch('/api/status')
            .then(response => response.json())
            .then(data => {
                document.getElementById('wifi-ssid').textContent = data.wifi_ssid;
                document.getElementById('ip-address').textContent = data.ip_address;
                document.getElementById('mqtt-broker').textContent = data.mqtt_broker;

                const mqttStatus = document.getElementById('mqtt-status');
                mqttStatus.textContent = data.mqtt_connected ? 'Conectado' : 'Desconectado';
                mqttStatus.className = data.mqtt_connected ? 'status-badge status-ok' : 'status-badge status-fail';

                const lastMqtt = document.getElementById('last-mqtt-success');
                lastMqtt.textContent = data.last_mqtt_success ? 'Sucesso' : 'Falhou';
                lastMqtt.className = data.last_mqtt_success ? 'status-badge status-ok' : 'status-badge status-fail';
            })
            .catch(error => console.error('Erro ao buscar status:', error));
    }

    function updateSensors() {
        fetch('/api/sensors')
            .then(response => response.json())
            .then(data => {
                // Atualiza análise de ML
                const airQuality = document.getElementById('air-quality');
                airQuality.textContent = data.air_quality || '--';
                airQuality.className = data.air_quality === 'Boa' ? 'status-ok' : (data.air_quality === 'Ruim' ? 'status-fail' : 'status-neutral');

                const anomaly = document.getElementById('anomaly');
                anomaly.textContent = data.anomaly ? 'SIM' : 'NÃO';
                anomaly.className = data.anomaly ? 'status-fail' : 'status-ok';

                // Atualiza grid de sensores
                const sensorsGrid = document.querySelector('.sensors-grid');
                sensorsGrid.innerHTML = ''; // Limpa o grid antes de preencher

                for (const key in SENSOR_CONFIG) {
                    if (data.hasOwnProperty(key)) {
                        const config = SENSOR_CONFIG[key];
                        const value = data[key];

                        const item = document.createElement('div');
                        item.className = 'sensor-item';
                        item.innerHTML = `
                            <div class="icon"><i class="${config.icon}"></i></div>
                            <div class="label">${config.label}</div>
                            <div class="value">${value.toFixed(2)} <span class="unit">${config.unit}</span></div>
                        `;
                        sensorsGrid.appendChild(item);
                    }
                }
            })
            .catch(error => console.error('Erro ao buscar dados dos sensores:', error));
    }

    // Chamar as funções de atualização imediatamente e depois a cada 5 segundos
    updateStatus();
    updateSensors();
    setInterval(updateStatus, 5000);
    setInterval(updateSensors, 5000);
});
