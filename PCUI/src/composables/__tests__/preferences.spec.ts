import { beforeEach, describe, expect, it, vi } from 'vitest'
import { defineComponent, nextTick } from 'vue'
import { mount } from '@vue/test-utils'
import { createPinia, setActivePinia } from 'pinia'
import PrimeVue, { usePrimeVue } from 'primevue/config'
import { i18n } from '@/locales'
import { useLocale } from '@/composables/useLocale'
import { useTheme } from '@/composables/useTheme'
import { useAppStore } from '@/stores/app'

const listeners: Array<() => void> = []
const media = { matches: false, addEventListener: vi.fn((_type, cb) => listeners.push(cb)), removeEventListener: vi.fn() }

describe('preference composables', () => {
  beforeEach(() => {
    localStorage.clear()
    document.documentElement.className = ''
    document.documentElement.lang = ''
    media.matches = false
    listeners.length = 0
    vi.stubGlobal('matchMedia', vi.fn(() => media))
    setActivePinia(createPinia())
  })

  it('applies dark and system themes to the document root', async () => {
    mount(defineComponent({ setup: useTheme, template: '<div />' }), { global: { plugins: [createPinia()] } })
    const store = useAppStore()
    store.setTheme('dark')
    await nextTick()
    expect(document.documentElement.classList.contains('app-dark')).toBe(true)
    store.setTheme('system')
    media.matches = false
    listeners.forEach((listener) => listener())
    expect(document.documentElement.classList.contains('app-dark')).toBe(false)
    media.matches = true
    listeners.forEach((listener) => listener())
    expect(document.documentElement.classList.contains('app-dark')).toBe(true)
  })

  it('synchronizes Vue I18n, PrimeVue locale, and html lang', async () => {
    const wrapper = mount(defineComponent({
      setup() {
        useLocale()
        return { primeVue: usePrimeVue() }
      },
      template: '<div :data-accept="primeVue.config.locale.accept" />',
    }), { global: { plugins: [createPinia(), i18n, PrimeVue] } })
    useAppStore().setLocale('en-US')
    await nextTick()
    expect(i18n.global.locale.value).toBe('en-US')
    expect(document.documentElement.lang).toBe('en-US')
    expect(wrapper.attributes('data-accept')).toBe('Yes')
    useAppStore().setLocale('zh-CN')
    await nextTick()
    expect(wrapper.attributes('data-accept')).toBe('\u786e\u5b9a')
  })
})
