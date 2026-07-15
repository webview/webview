<script setup lang="ts">
withDefaults(defineProps<{
  loading?: boolean
  error?: string | null
  empty?: boolean
  emptyIcon?: string
}>(), {
  loading: false,
  error: null,
  empty: false,
  emptyIcon: 'pi pi-inbox',
})

defineEmits<{ retry: [] }>()
</script>

<template>
  <div v-if="loading" data-testid="loading" class="space-y-3" aria-live="polite">
    <div v-for="index in 3" :key="index" class="h-20 animate-pulse rounded-2xl bg-emphasis" />
    <span class="sr-only">{{ $t('common.loading') }}</span>
  </div>
  <div v-else-if="error" class="grid min-h-52 place-items-center rounded-2xl border border-surface bg-surface-0 p-8 text-center dark:bg-surface-900">
    <div>
      <i class="pi pi-cloud-off mb-3 text-2xl text-red-500" aria-hidden="true" />
      <p class="mb-4 mt-0 text-sm text-muted-color">{{ error }}</p>
      <button data-testid="retry" type="button" class="rounded-xl bg-primary px-4 py-2 text-sm font-medium text-primary-contrast" @click="$emit('retry')">
        {{ $t('common.retry') }}
      </button>
    </div>
  </div>
  <div v-else-if="empty" data-testid="empty" class="grid min-h-52 place-items-center rounded-2xl border border-dashed border-surface p-8 text-center">
    <div class="text-muted-color">
      <i :class="emptyIcon" class="mb-3 text-2xl" aria-hidden="true" />
      <p class="m-0 text-sm">{{ $t('common.empty') }}</p>
    </div>
  </div>
  <slot v-else />
</template>
