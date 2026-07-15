<script setup lang="ts">
import { ref } from 'vue'
import Avatar from 'primevue/avatar'
import Button from 'primevue/button'
import InputText from 'primevue/inputtext'
import PageHeader from '@/components/common/PageHeader.vue'

const selected = ref(0)
const messages = [{ sender: 'Zhou Yu', initials: 'Z', title: 'Northstar prototype review', preview: 'I added the missing interaction states for review.', time: '10:24', unread: true }, { sender: 'Shen Xing', initials: 'S', title: 'Quarterly data report', preview: 'The growth section now includes the latest data.', time: 'Yesterday', unread: true }, { sender: 'Product Team', initials: 'P', title: 'Weekly sync notes', preview: 'Meeting notes and follow-up actions are ready.', time: 'Monday', unread: false }]
</script>

<template>
  <PageHeader :title="$t('messages.title')" :description="$t('messages.description')"><template #actions><Button :label="$t('messages.compose')" icon="pi pi-pencil" /></template></PageHeader>
  <section class="grid min-h-[560px] overflow-hidden rounded-2xl border border-surface bg-surface-0 dark:bg-surface-900 lg:grid-cols-[320px_minmax(0,1fr)]"><aside class="border-b border-surface lg:border-b-0 lg:border-r"><div class="p-3"><span class="p-input-icon-left w-full"><i class="pi pi-search" /><InputText class="w-full" :placeholder="$t('messages.search')" /></span></div><button v-for="(message,index) in messages" :key="message.title" class="flex w-full gap-3 border-0 border-t border-surface bg-transparent p-4 text-left" :class="{ 'bg-emphasis': selected === index }" @click="selected=index"><Avatar :label="message.initials" shape="circle" /><span class="min-w-0 flex-1"><span class="flex justify-between gap-2"><strong class="truncate text-sm font-medium">{{ message.sender }}</strong><small class="text-muted-color">{{ message.time }}</small></span><strong class="mt-1 block truncate text-sm" :class="{ 'font-medium': message.unread }">{{ message.title }}</strong><small class="mt-1 block truncate text-muted-color">{{ message.preview }}</small></span><span v-if="message.unread" class="mt-2 size-2 rounded-full bg-primary" /></button></aside><article class="p-6 lg:p-8"><div class="flex items-start justify-between border-b border-surface pb-5"><div><p class="m-0 text-sm text-muted-color">{{ messages[selected]?.sender }}</p><h2 class="mb-0 mt-1 text-xl font-medium">{{ messages[selected]?.title }}</h2></div><Button icon="pi pi-ellipsis-h" text rounded /></div><div class="max-w-2xl py-7 text-[15px] leading-7"><p>{{ messages[selected]?.preview }}</p><p>{{ $t('messages.body') }}</p></div><div class="rounded-xl bg-emphasis p-4 text-sm text-muted-color"><i class="pi pi-paperclip mr-2" />Product-review-notes.pdf · 2.4 MB</div></article></section>
</template>
