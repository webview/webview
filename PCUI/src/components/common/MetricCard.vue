<script setup lang="ts">
withDefaults(defineProps<{
  label: string
  value: string | number
  icon: string
  helper?: string
  tone?: 'primary' | 'success' | 'warning' | 'violet'
}>(), { tone: 'primary', helper: '' })
</script>

<template>
  <article class="flex min-w-0 gap-3 rounded-2xl border border-surface bg-surface-0 p-4 dark:bg-surface-900">
    <span class="metric-icon" :data-tone="tone"><i :class="icon" aria-hidden="true" /></span>
    <div class="grid min-w-0 gap-0.5">
      <span class="truncate text-sm text-muted-color">{{ label }}</span>
      <strong class="truncate text-2xl font-medium tracking-tight">{{ value }}</strong>
      <small v-if="helper" class="truncate text-xs text-muted-color">{{ helper }}</small>
    </div>
  </article>
</template>
