import { computed, watch } from 'vue'
import { storeToRefs } from 'pinia'
import { usePrimeVue } from 'primevue/config'
import { i18n } from '@/locales'
import { primeVueLocales } from '@/locales/primevue'
import { useAppStore, type AppLocale } from '@/stores/app'

export const useLocale = () => {
  const store = useAppStore()
  const primeVue = usePrimeVue()
  const { locale } = storeToRefs(store)

  watch(locale, (value) => {
    i18n.global.locale.value = value
    Object.assign(primeVue.config.locale, primeVueLocales[value])
    document.documentElement.lang = value
  }, { immediate: true })

  return {
    locale,
    locales: computed(() => [
      { label: '简体中文', value: 'zh-CN' as AppLocale },
      { label: 'English', value: 'en-US' as AppLocale },
    ]),
    setLocale: store.setLocale,
  }
}
