<script setup lang="ts">
import { onBeforeUnmount, onMounted, ref } from 'vue'
import { windowBridge } from '@/bridge/windowBridge'

const maximized = ref(false)
let dispose: (() => void) | undefined

onMounted(async () => {
  maximized.value = await windowBridge.isMaximized()
  dispose = windowBridge.onWindowStateChanged((value) => (maximized.value = value))
})

onBeforeUnmount(() => dispose?.())

const toggleMaximize = async () => {
  await windowBridge.toggleMaximize()
  maximized.value = !maximized.value
}
</script>

<template>
  <div class="flex h-full items-center border-l border-surface px-1" :aria-label="$t('window.controls')">
    <button type="button" class="window-control" :aria-label="$t('window.minimize')" @click="windowBridge.minimize">
      <i class="pi pi-minus" aria-hidden="true" />
    </button>
    <button type="button" class="window-control" :aria-label="$t(maximized ? 'window.restore' : 'window.maximize')" @click="toggleMaximize">
      <i :class="maximized ? 'pi pi-clone' : 'pi pi-stop'" aria-hidden="true" />
    </button>
    <button type="button" class="window-control window-close" :aria-label="$t('window.close')" @click="windowBridge.close">
      <i class="pi pi-times" aria-hidden="true" />
    </button>
  </div>
</template>
