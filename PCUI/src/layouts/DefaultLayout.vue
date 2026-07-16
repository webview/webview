<script setup lang="ts">
import { onBeforeUnmount, onMounted, watch } from 'vue'
import { useRoute } from 'vue-router'
import { storeToRefs } from 'pinia'
import AppSidebar from '@/components/shell/AppSidebar.vue'
import AppTopbar from '@/components/shell/AppTopbar.vue'
import RightContextPanel from '@/components/shell/RightContextPanel.vue'
import { useAppStore } from '@/stores/app'

const appStore = useAppStore()
const route = useRoute()
const { rightPanelOpen } = storeToRefs(appStore)

const compactQuery = window.matchMedia('(max-width: 1100px)')
const syncCompactViewport = () => appStore.setCompactViewport(compactQuery.matches)
onMounted(() => {
  syncCompactViewport()
  compactQuery.addEventListener('change', syncCompactViewport)
})
onBeforeUnmount(() => compactQuery.removeEventListener('change', syncCompactViewport))
watch(() => route.meta.contextPanel, (open) => { rightPanelOpen.value = Boolean(open) }, { immediate: true })
</script>

<template>
  <div class="flex min-h-dvh bg-app text-color">
    <AppSidebar />
    <div class="flex min-w-0 flex-1 flex-col">
      <AppTopbar />
      <div class="flex min-h-0 flex-1">
        <main class="min-w-0 flex-1 overflow-auto p-5 lg:p-7">
          <RouterView />
        </main>
        <RightContextPanel :open="rightPanelOpen" @close="rightPanelOpen = false" />
      </div>
    </div>
  </div>
</template>
