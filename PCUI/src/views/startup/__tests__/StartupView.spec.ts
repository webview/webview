import { afterEach, beforeEach, describe, expect, it, vi } from 'vitest'
import { nextTick } from 'vue'
import { flushPromises, mount, type VueWrapper } from '@vue/test-utils'
import { createPinia, setActivePinia } from 'pinia'
import { createMemoryHistory, createRouter } from 'vue-router'
import StartupView from '@/views/startup/StartupView.vue'
import { i18n } from '@/locales'
import { useStartupStore } from '@/stores/startup'
import { runStartupInitialization } from '@/services/initializeApp'
import { windowBridge } from '@/bridge/windowBridge'

vi.mock('@/services/initializeApp', () => ({
  runStartupInitialization: vi.fn(),
}))

vi.mock('@/bridge/windowBridge', () => ({
  windowBridge: { close: vi.fn() },
}))

const wrappers: VueWrapper[] = []

const mountStartup = async () => {
  const pinia = createPinia()
  setActivePinia(pinia)
  const router = createRouter({
    history: createMemoryHistory(),
    routes: [
      { path: '/startup', name: 'startup', component: StartupView },
      { path: '/dashboard', name: 'dashboard', component: { template: '<div>Dashboard</div>' } },
    ],
  })
  await router.push('/startup')
  await router.isReady()
  const wrapper = mount(StartupView, {
    attachTo: document.body,
    global: { plugins: [pinia, router, i18n] },
  })
  wrappers.push(wrapper)
  await nextTick()
  return { wrapper, router, store: useStartupStore() }
}

describe('startup view', () => {
  beforeEach(() => {
    vi.clearAllMocks()
    i18n.global.locale.value = 'zh-CN'
    vi.mocked(runStartupInitialization).mockImplementation(async ({ store }) => {
      store.begin()
      return false
    })
  })

  afterEach(() => {
    wrappers.splice(0).forEach((wrapper) => wrapper.unmount())
    document.body.innerHTML = ''
  })

  it('renders the full-window loading state and starts initialization once', async () => {
    const { wrapper } = await mountStartup()

    expect(runStartupInitialization).toHaveBeenCalledTimes(1)
    expect(wrapper.get('[role="progressbar"]').attributes('aria-valuenow')).toBe('0')
    expect(wrapper.find('.startup-screen').exists()).toBe(true)
    expect(wrapper.find('.startup-brand').exists()).toBe(true)
    expect(wrapper.find('.startup-content').exists()).toBe(true)
    expect(wrapper.find('.startup-state-label').exists()).toBe(false)
  })

  it('shows a recoverable failure, keeps the icon beside the English status, and focuses retry', async () => {
    const { wrapper, store } = await mountStartup()

    store.fail('HOST_TIMEOUT', 'startup.errors.timeout')
    await nextTick()
    await nextTick()

    const alert = wrapper.get('[role="alert"]')
    expect(alert.text()).toContain('HOST_TIMEOUT')
    expect(wrapper.get('[data-testid="startup-error-kicker"]').text()).toContain('STARTUP INTERRUPTED')
    expect(document.activeElement).toBe(wrapper.get('[data-testid="startup-retry"]').element)

    await wrapper.get('[data-testid="startup-retry"]').trigger('click')
    expect(runStartupInitialization).toHaveBeenCalledTimes(2)
  })

  it('uses the window bridge for both close controls', async () => {
    const { wrapper, store } = await mountStartup()

    store.fail('HOST_TIMEOUT', 'startup.errors.timeout')
    await nextTick()
    await wrapper.get('[data-testid="startup-close"]').trigger('click')
    await wrapper.get('[data-testid="startup-exit"]').trigger('click')

    expect(windowBridge.close).toHaveBeenCalledTimes(2)
  })

  it('replaces startup with dashboard after successful initialization', async () => {
    vi.mocked(runStartupInitialization).mockImplementationOnce(async ({ store }) => {
      store.begin()
      store.succeed()
      return true
    })
    const { router } = await mountStartup()
    await flushPromises()

    expect(router.currentRoute.value.name).toBe('dashboard')
  })
})
