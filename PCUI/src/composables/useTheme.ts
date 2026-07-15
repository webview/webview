import { onBeforeUnmount, watch } from 'vue'
import { storeToRefs } from 'pinia'
import { useAppStore } from '@/stores/app'

export const useTheme = () => {
  const store = useAppStore()
  const { theme } = storeToRefs(store)
  const systemTheme = window.matchMedia('(prefers-color-scheme: dark)')

  const applyTheme = () => {
    const dark = theme.value === 'dark' || (theme.value === 'system' && systemTheme.matches)
    document.documentElement.classList.toggle('app-dark', dark)
  }

  const onSystemChange = () => theme.value === 'system' && applyTheme()
  systemTheme.addEventListener('change', onSystemChange)
  const stop = watch(theme, applyTheme, { immediate: true })

  onBeforeUnmount(() => {
    stop()
    systemTheme.removeEventListener('change', onSystemChange)
  })

  return { theme, setTheme: store.setTheme }
}
