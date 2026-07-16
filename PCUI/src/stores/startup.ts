import { computed, ref } from 'vue'
import { defineStore } from 'pinia'

export type StartupStatus = 'idle' | 'running' | 'success' | 'failed'
export type StartupStepKey = 'preferences' | 'profile' | 'workspace' | 'host'

export const useStartupStore = defineStore('startup', () => {
  const status = ref<StartupStatus>('idle')
  const currentStep = ref<StartupStepKey | null>(null)
  const completedSteps = ref<StartupStepKey[]>([])
  const progress = ref(0)
  const errorCode = ref<string | null>(null)
  const errorMessageKey = ref<string | null>(null)
  const attempt = ref(0)
  const initialized = computed(() => status.value === 'success')

  const begin = () => {
    if (status.value === 'running') return false
    status.value = 'running'
    currentStep.value = null
    completedSteps.value = []
    progress.value = 0
    errorCode.value = null
    errorMessageKey.value = null
    attempt.value += 1
    return true
  }

  const startStep = (step: StartupStepKey) => {
    currentStep.value = step
  }

  const advance = (step: StartupStepKey, value: number) => {
    if (!completedSteps.value.includes(step)) completedSteps.value.push(step)
    progress.value = Math.max(progress.value, Math.min(100, value))
  }

  const succeed = () => {
    status.value = 'success'
    progress.value = 100
  }

  const fail = (code: string, messageKey: string) => {
    status.value = 'failed'
    errorCode.value = code
    errorMessageKey.value = messageKey
  }

  return {
    status,
    currentStep,
    completedSteps,
    progress,
    errorCode,
    errorMessageKey,
    attempt,
    initialized,
    begin,
    startStep,
    advance,
    succeed,
    fail,
  }
})
