<script setup lang="ts">
import { useRoute } from 'vue-router'
const route = useRoute()
</script>

<template>
  <section class="rounded-2xl border border-surface bg-surface-0 p-8 dark:bg-surface-900">
    <p class="mb-2 text-sm text-primary">{{ $t('app.workspace') }}</p>
    <h1 class="m-0 text-2xl font-medium">{{ $t(route.meta.titleKey) }}</h1>
  </section>
</template>
