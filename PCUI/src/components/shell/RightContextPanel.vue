<script setup lang="ts">
defineProps<{ open: boolean }>()
defineEmits<{ close: [] }>()
</script>

<template>
  <Transition name="panel">
    <aside v-if="open" class="w-80 shrink-0 border-l border-surface bg-surface-0 p-5 dark:bg-surface-900">
      <div class="mb-4 flex items-center justify-between">
        <strong>{{ $t('assistant.context') }}</strong>
        <button type="button" class="sidebar-icon-button" :aria-label="$t('actions.close')" @click="$emit('close')">
          <i class="pi pi-times" aria-hidden="true" />
        </button>
      </div>
      <slot />
    </aside>
  </Transition>
</template>
