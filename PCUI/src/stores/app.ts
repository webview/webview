import { defineStore } from 'pinia'
import { ref } from 'vue'

export type ThemeMode = 'light' | 'dark' | 'system'
export type AppLocale = 'zh-CN' | 'en-US'

const readBoolean = (key: string, fallback = false) => {
  const value = localStorage.getItem(key)
  return value === null ? fallback : value === 'true'
}

const readChoice = <T extends string>(key: string, values: readonly T[], fallback: T) => {
  const value = localStorage.getItem(key)
  return values.includes(value as T) ? (value as T) : fallback
}

export const useAppStore = defineStore('app', () => {
  const sidebarCollapsed = ref(readBoolean('pcui:sidebar-collapsed'))
  const rightPanelOpen = ref(false)
  const theme = ref<ThemeMode>(
    readChoice('pcui:theme', ['light', 'dark', 'system'] as const, 'light'),
  )
  const locale = ref<AppLocale>(
    readChoice('pcui:locale', ['zh-CN', 'en-US'] as const, 'zh-CN'),
  )

  const setSidebarCollapsed = (value: boolean) => {
    sidebarCollapsed.value = value
    localStorage.setItem('pcui:sidebar-collapsed', String(value))
  }

  const toggleSidebar = () => setSidebarCollapsed(!sidebarCollapsed.value)

  const setTheme = (value: ThemeMode) => {
    theme.value = value
    localStorage.setItem('pcui:theme', value)
  }

  const setLocale = (value: AppLocale) => {
    locale.value = value
    localStorage.setItem('pcui:locale', value)
  }

  return {
    sidebarCollapsed,
    rightPanelOpen,
    theme,
    locale,
    setSidebarCollapsed,
    toggleSidebar,
    setTheme,
    setLocale,
  }
})
