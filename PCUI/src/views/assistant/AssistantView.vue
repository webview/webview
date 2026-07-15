<script setup lang="ts">
import { nextTick, ref } from 'vue'
import Button from 'primevue/button'
import Textarea from 'primevue/textarea'
import PageHeader from '@/components/common/PageHeader.vue'
import { getAssistantReply, type AssistantMessage } from '@/services/assistant'

const input = ref('')
const sending = ref(false)
const messages = ref<AssistantMessage[]>([{ id: 1, role: 'assistant', content: '你好，我可以帮助你整理项目、总结信息或规划今天的工作。' }])
const prompts = ['总结本周项目进度', '生成今日工作计划', '分析任务优先级']
const send = async (preset?: string) => {
  const content = (preset || input.value).trim()
  if (!content || sending.value) return
  messages.value.push({ id: Date.now(), role: 'user', content }); input.value = ''; sending.value = true
  await nextTick()
  messages.value.push(await getAssistantReply(content)); sending.value = false
}
</script>

<template>
  <PageHeader :title="$t('assistantPage.title')" :description="$t('assistantPage.description')" />
  <section class="mx-auto flex min-h-[620px] max-w-5xl flex-col overflow-hidden rounded-2xl border border-surface bg-surface-0 dark:bg-surface-900"><div class="flex-1 space-y-5 overflow-auto p-6 lg:p-8"><div v-for="message in messages" :key="message.id" class="flex gap-3" :class="{ 'justify-end': message.role === 'user' }"><span v-if="message.role === 'assistant'" class="grid size-9 shrink-0 place-items-center rounded-xl bg-primary text-primary-contrast"><i class="pi pi-sparkles" /></span><p class="m-0 max-w-[75%] rounded-2xl px-4 py-3 text-sm leading-6" :class="message.role === 'user' ? 'bg-primary text-primary-contrast' : 'bg-emphasis'">{{ message.content }}</p></div><p v-if="sending" class="text-sm text-muted-color">{{ $t('assistantPage.thinking') }}</p></div><div class="border-t border-surface p-4"><div class="mb-3 flex flex-wrap gap-2"><Button v-for="prompt in prompts" :key="prompt" :label="prompt" size="small" outlined rounded @click="send(prompt)" /></div><div class="flex items-end gap-2 rounded-2xl border border-surface p-2 focus-within:border-primary"><Textarea v-model="input" auto-resize rows="2" class="flex-1 border-0 shadow-none" :placeholder="$t('assistantPage.placeholder')" @keydown.ctrl.enter="send()" /><Button icon="pi pi-arrow-up" rounded :loading="sending" :aria-label="$t('assistantPage.send')" @click="send()" /></div><p class="mb-0 mt-2 text-center text-xs text-muted-color">{{ $t('assistantPage.hint') }}</p></div></section>
</template>
