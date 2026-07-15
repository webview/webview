<script setup lang="ts">
import Tag from 'primevue/tag'

defineProps<{ label: string; severity?: 'success' | 'info' | 'warn' | 'danger' | 'secondary' }>()
</script>

<template><Tag :value="label" :severity="severity || 'secondary'" rounded /></template>
