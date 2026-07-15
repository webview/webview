<script setup lang="ts">
import { storeToRefs } from 'pinia'
import AppSidebar from '@/components/shell/AppSidebar.vue'
import AppTopbar from '@/components/shell/AppTopbar.vue'
import RightContextPanel from '@/components/shell/RightContextPanel.vue'
import { useAppStore } from '@/stores/app'
import { useLocale } from '@/composables/useLocale'
import { useTheme } from '@/composables/useTheme'

const appStore = useAppStore()
const { rightPanelOpen } = storeToRefs(appStore)
useTheme()
useLocale()
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
