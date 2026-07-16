<script setup lang="ts">
import { onErrorCaptured } from 'vue'
import ConfirmDialog from 'primevue/confirmdialog'
import Toast from 'primevue/toast'
import { useToast } from 'primevue/usetoast'
import { useI18n } from 'vue-i18n'
import { useLocale } from '@/composables/useLocale'
import { useTheme } from '@/composables/useTheme'

const toast = useToast()
const { t } = useI18n()
useTheme()
useLocale()
onErrorCaptured((error) => {
  console.error(error)
  toast.add({ severity: 'error', summary: t('errors.runtimeTitle'), detail: t('errors.runtimeHelp'), life: 5000 })
  return false
})
</script>

<template>
  <Toast position="bottom-right" />
  <ConfirmDialog />
  <RouterView />
</template>
