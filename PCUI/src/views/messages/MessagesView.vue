<script setup lang="ts">
import { computed, onMounted, ref } from 'vue'
import Avatar from 'primevue/avatar'; import Button from 'primevue/button'; import InputText from 'primevue/inputtext'
import AsyncContent from '@/components/common/AsyncContent.vue'; import PageHeader from '@/components/common/PageHeader.vue'
import { useAsyncData } from '@/composables/useAsyncData'; import { getMessages } from '@/services/workspace'

const selectedId = ref(1)
const { data: messages, loading, error, load } = useAsyncData(getMessages)
const selected = computed(() => messages.value?.find(item=>item.id===selectedId.value))
const selectMessage = (id:number) => { selectedId.value=id; const item=messages.value?.find(message=>message.id===id); if(item) item.unread=false }
onMounted(load)
</script>

<template><PageHeader :title="$t('messages.title')" :description="$t('messages.description')"><template #actions><Button :label="$t('messages.compose')" icon="pi pi-pencil" /></template></PageHeader><AsyncContent :loading="loading" :error="error" :empty="!messages?.length" @retry="load"><section class="grid min-h-[560px] overflow-hidden rounded-2xl border border-surface bg-surface-0 dark:bg-surface-900 lg:grid-cols-[320px_minmax(0,1fr)]"><aside class="border-r border-surface"><div class="p-3"><span class="p-input-icon-left w-full"><i class="pi pi-search" /><InputText class="w-full" :placeholder="$t('messages.search')" /></span></div><button v-for="message in messages" :key="message.id" class="flex w-full gap-3 border-0 border-t border-surface bg-transparent p-4 text-left" :class="{'bg-emphasis':selectedId===message.id}" @click="selectMessage(message.id)"><Avatar :label="message.initials" shape="circle" /><span class="min-w-0 flex-1"><span class="flex justify-between"><strong>{{ $t(message.senderKey) }}</strong><small>{{ $t(message.timeKey) }}</small></span><strong class="mt-1 block truncate">{{ $t(message.titleKey) }}</strong><small class="block truncate text-muted-color">{{ $t(message.previewKey) }}</small></span><span v-if="message.unread" class="mt-2 size-2 rounded-full bg-primary" /></button></aside><article class="p-6 lg:p-8"><div class="flex justify-between border-b border-surface pb-5"><div><p class="m-0 text-sm text-muted-color">{{ selected ? $t(selected.senderKey) : '' }}</p><h2 class="mb-0 mt-1 text-xl font-medium">{{ selected ? $t(selected.titleKey) : '' }}</h2></div><Button icon="pi pi-ellipsis-h" text rounded /></div><div class="max-w-2xl py-7 leading-7"><p>{{ selected ? $t(selected.previewKey) : '' }}</p><p>{{ $t('messages.body') }}</p></div></article></section></AsyncContent></template>
