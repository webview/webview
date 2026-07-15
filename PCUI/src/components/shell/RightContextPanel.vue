<script setup lang="ts">
import { computed, onBeforeUnmount, onMounted, ref } from 'vue'
import Drawer from 'primevue/drawer'

const props = defineProps<{ open: boolean }>()
const emit = defineEmits<{ close: [] }>()
const query = window.matchMedia('(max-width: 1100px)')
const compact = ref(query.matches)
const visible = computed({ get: () => props.open, set: (value) => { if (!value) emit('close') } })
const sync = () => { compact.value = query.matches }
onMounted(() => query.addEventListener('change', sync))
onBeforeUnmount(() => query.removeEventListener('change', sync))
</script>

<template>
  <Drawer v-if="compact" v-model:visible="visible" position="right" class="w-full sm:w-80" :header="$t('assistant.context')">
    <slot><p class="text-sm leading-6 text-muted-color">{{ $t('assistant.contextHelp') }}</p></slot>
  </Drawer>
  <Transition v-else name="panel">
    <aside v-if="open" class="w-80 shrink-0 border-l border-surface bg-surface-0 p-5 dark:bg-surface-900">
      <div class="mb-4 flex items-center justify-between"><strong>{{ $t('assistant.context') }}</strong><button type="button" class="sidebar-icon-button" :aria-label="$t('actions.close')" @click="$emit('close')"><i class="pi pi-times" aria-hidden="true" /></button></div>
      <slot><p class="text-sm leading-6 text-muted-color">{{ $t('assistant.contextHelp') }}</p></slot>
    </aside>
  </Transition>
</template>
