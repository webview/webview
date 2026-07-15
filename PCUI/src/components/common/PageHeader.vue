<script setup lang="ts">
defineProps<{
  eyebrow?: string
  title: string
  description?: string
}>()
</script>

<template>
  <header class="mb-6 flex flex-wrap items-end justify-between gap-4">
    <div>
      <p v-if="eyebrow" class="mb-1 mt-0 text-sm text-muted-color">{{ eyebrow }}</p>
      <h1 class="m-0 text-2xl font-medium tracking-tight lg:text-[28px]">{{ title }}</h1>
      <p v-if="description" class="mb-0 mt-1.5 text-sm text-muted-color">{{ description }}</p>
    </div>
    <div class="flex flex-wrap items-center gap-2"><slot name="actions" /></div>
  </header>
</template>
