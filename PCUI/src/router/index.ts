import { createRouter, createWebHashHistory, type RouterHistory } from 'vue-router'
import { useStartupStore } from '@/stores/startup'
import { routes } from './routes'

export const createAppRouter = (history: RouterHistory = createWebHashHistory()) => {
  const router = createRouter({
    history,
    routes,
    scrollBehavior: () => ({ top: 0 }),
  })

  router.beforeEach((to) => {
    const startup = useStartupStore()
    if (to.name === 'startup' && startup.initialized) return { name: 'dashboard' }
    if (!to.meta.public && !startup.initialized) return { name: 'startup' }
  })

  return router
}

export const router = createAppRouter()
