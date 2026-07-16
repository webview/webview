<script setup lang="ts">
import { nextTick, onMounted, ref, watch } from 'vue'
import { useI18n } from 'vue-i18n'
import { useRouter } from 'vue-router'
import { windowBridge } from '@/bridge/windowBridge'
import { runStartupInitialization } from '@/services/initializeApp'
import { useStartupStore } from '@/stores/startup'

const startup = useStartupStore()
const router = useRouter()
const { t } = useI18n()
const retryButton = ref<HTMLButtonElement | null>(null)

const run = async () => {
  const succeeded = await runStartupInitialization({ store: startup })
  if (succeeded) await router.replace({ name: 'dashboard' })
}

watch(() => startup.status, async (status) => {
  if (status !== 'failed') return
  await nextTick()
  retryButton.value?.focus()
})

onMounted(run)
</script>

<template>
  <main class="startup-screen" :class="{ 'is-failed': startup.status === 'failed' }">
    <section class="startup-brand" data-webview-drag-region aria-hidden="true">
      <span class="startup-orbit" />
      <span class="startup-orbit startup-orbit-secondary" />
      <span class="startup-glow startup-glow-one" />
      <span class="startup-glow startup-glow-two" />
      <div class="startup-brand-mark">
        <span class="startup-logo">✦</span>
        <strong>Northstar</strong>
        <small>Desktop Workspace</small>
      </div>
    </section>

    <section class="startup-content">
      <div class="startup-drag-region" data-webview-drag-region />
      <button
        type="button"
        class="startup-close"
        data-testid="startup-close"
        :aria-label="t('startup.close')"
        @click="windowBridge.close"
      >
        <i class="pi pi-times" aria-hidden="true" />
      </button>

      <div v-if="startup.status !== 'failed'" class="startup-state" aria-live="polite">
        <span class="startup-eyebrow">{{ t('startup.eyebrow') }}</span>
        <h1>{{ t('startup.heading') }}</h1>
        <p>{{ t('startup.description') }}</p>
        <div class="startup-progress-copy">
          <strong>{{ t(`startup.steps.${startup.currentStep ?? 'preferences'}`) }}</strong>
          <span>{{ startup.progress }}%</span>
        </div>
        <div
          class="startup-progress"
          role="progressbar"
          aria-valuemin="0"
          aria-valuemax="100"
          :aria-valuenow="startup.progress"
        >
          <span :style="{ width: `${startup.progress}%` }" />
        </div>
      </div>

      <div v-else class="startup-state" role="alert">
        <div class="startup-error-kicker" data-testid="startup-error-kicker">
          <span class="startup-error-icon" aria-hidden="true">!</span>
          <span class="startup-eyebrow">{{ t('startup.interrupted') }}</span>
        </div>
        <h1>{{ t('startup.failedTitle') }}</h1>
        <p>{{ t(startup.errorMessageKey ?? 'startup.errors.generic') }}</p>
        <code class="startup-error-code">
          {{ t('startup.errorCode', { code: startup.errorCode ?? 'STARTUP_FAILED' }) }}
        </code>
        <div class="startup-actions">
          <button ref="retryButton" type="button" data-testid="startup-retry" @click="run">
            {{ t('startup.retry') }}
          </button>
          <button type="button" data-testid="startup-exit" @click="windowBridge.close">
            {{ t('startup.exit') }}
          </button>
        </div>
      </div>
    </section>
  </main>
</template>
