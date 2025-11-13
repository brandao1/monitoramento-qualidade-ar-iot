<script setup lang="ts">
import { ref, onMounted } from 'vue'
import axios from 'axios'

const sensorData = ref([])

onMounted(async () => {
  try {
    const response = await axios.post('http://localhost:4200/_sql', {
      stmt: 'SELECT * FROM mtcaps ORDER BY time_index DESC LIMIT 100'
    })
    sensorData.value = response.data.rows
  } catch (error) {
    console.error('Error fetching data from CrateDB:', error)
  }
})
</script>

<template>
  <div>
    <h1>Sensor Data</h1>
    <table>
      <thead>
        <tr>
          <th>Entity ID</th>
          <th>Entity Type</th>
          <th>Time Index</th>
          <th>Temperature</th>
          <th>Humidity</th>
          <th>Air Quality</th>
        </tr>
      </thead>
      <tbody>
        <tr v-for="row in sensorData" :key="row[0]">
          <td>{{ row[0] }}</td>
          <td>{{ row[1] }}</td>
          <td>{{ row[2] }}</td>
          <td>{{ row[3] }}</td>
          <td>{{ row[4] }}</td>
          <td>{{ row[5] }}</td>
        </tr>
      </tbody>
    </table>
  </div>
</template>

<style scoped>
table {
  width: 100%;
  border-collapse: collapse;
}

th,
td {
  border: 1px solid #ddd;
  padding: 8px;
}

th {
  background-color: #f2f2f2;
}
</style>
