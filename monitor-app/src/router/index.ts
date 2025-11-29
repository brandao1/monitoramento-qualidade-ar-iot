import { createRouter, createWebHistory } from 'vue-router'
import Dashboard from '@/views/Dashboard.vue'
import TemporalView from '@/views/TemporalView.vue'
import ReferenceValues from '@/views/ReferenceValues.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'dashboard',
      component: Dashboard,
    },
    {
      path: '/temporal',
      name: 'temporal',
      component: TemporalView,
    },
    {
      path: '/reference',
      name: 'reference',
      component: ReferenceValues,
    },
  ],
})

export default router
