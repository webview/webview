<script setup lang="ts">
import { useLocale } from '@/composables/useLocale'
import { useTheme } from '@/composables/useTheme'
useTheme()
useLocale()
</script>

<template>
  <main class="relative grid min-h-dvh place-items-center overflow-hidden bg-app p-6 text-color">
    <div class="pointer-events-none absolute -left-24 -top-24 size-96 rounded-full bg-primary/10 blur-3xl" />
    <div class="pointer-events-none absolute -bottom-32 -right-24 size-96 rounded-full bg-indigo-300/10 blur-3xl" />
    <div class="relative w-full max-w-md">
      <RouterView />
    </div>
  </main>
</template>
