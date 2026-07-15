import { beforeEach, describe, expect, it } from 'vitest'
import { createPinia, setActivePinia } from 'pinia'
import { useAppStore } from '@/stores/app'

describe('app store', () => {
  beforeEach(() => {
    localStorage.clear()
    setActivePinia(createPinia())
  })

  it('toggles the sidebar and persists the preference', () => {
    const store = useAppStore()

    store.toggleSidebar()

    expect(store.sidebarCollapsed).toBe(true)
    expect(localStorage.getItem('pcui:sidebar-collapsed')).toBe('true')
  })

  it('stores theme and locale selections', () => {
    const store = useAppStore()

    store.setTheme('dark')
    store.setLocale('en-US')

    expect(store.theme).toBe('dark')
    expect(store.locale).toBe('en-US')
    expect(localStorage.getItem('pcui:theme')).toBe('dark')
    expect(localStorage.getItem('pcui:locale')).toBe('en-US')
  })
})
