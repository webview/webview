import { beforeEach, describe, expect, it } from 'vitest'
import { createPinia, setActivePinia } from 'pinia'
import { createMemoryHistory } from 'vue-router'
import { createAppRouter } from '@/router'
import { useStartupStore } from '@/stores/startup'

describe('startup routing', () => {
  beforeEach(() => setActivePinia(createPinia()))

  it('uses startup as the cold default entry', async () => {
    const router = createAppRouter(createMemoryHistory())
    await router.push('/')
    await router.isReady()

    expect(router.currentRoute.value.name).toBe('startup')
  })

  it('redirects protected workspace routes until startup succeeds', async () => {
    const router = createAppRouter(createMemoryHistory())
    await router.push('/projects')

    expect(router.currentRoute.value.name).toBe('startup')
  })

  it('allows workspace routes after startup succeeds', async () => {
    const router = createAppRouter(createMemoryHistory())
    const startup = useStartupStore()
    startup.begin()
    startup.succeed()
    await router.push('/projects')

    expect(router.currentRoute.value.name).toBe('projects')
  })

  it('keeps auth routes public', async () => {
    const router = createAppRouter(createMemoryHistory())
    await router.push('/auth/login')

    expect(router.currentRoute.value.name).toBe('login')
  })
})
