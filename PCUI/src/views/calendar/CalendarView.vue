<script setup lang="ts">
import { ref } from 'vue'
import Button from 'primevue/button'
import DatePicker from 'primevue/datepicker'
import Dialog from 'primevue/dialog'
import InputText from 'primevue/inputtext'
import PageHeader from '@/components/common/PageHeader.vue'

const dialogOpen = ref(false)
const days = Array.from({ length: 35 }, (_, index) => ({ date: index - 1, current: index >= 2 && index < 33, events: index === 5 ? ['Product review'] : index === 12 ? ['Team sync','Release check'] : index === 22 ? ['User interview'] : [] }))
const upcoming = [{ time: '10:00', title: 'Prototype review', color: '#4f6bed' }, { time: '14:30', title: 'Design system sync', color: '#16a36a' }, { time: '16:30', title: 'Team sync', color: '#e59a18' }]
</script>

<template>
  <PageHeader :title="$t('calendar.title')" :description="$t('calendar.description')"><template #actions><Button :label="$t('calendar.new')" icon="pi pi-plus" @click="dialogOpen = true" /></template></PageHeader>
  <div class="grid gap-3 xl:grid-cols-[minmax(0,1fr)_300px]"><section class="workspace-panel"><div class="mb-4 flex items-center justify-between"><div><Button icon="pi pi-angle-left" text rounded /><Button :label="$t('calendar.today')" text /><Button icon="pi pi-angle-right" text rounded /></div><strong>{{ $t('calendar.month') }}</strong><Button icon="pi pi-ellipsis-h" text rounded /></div><div class="grid grid-cols-7 border-l border-t border-surface"><div v-for="day in $t('calendar.weekdays').split(',')" :key="day" class="border-b border-r border-surface p-2 text-center text-xs text-muted-color">{{ day }}</div><button v-for="(day,index) in days" :key="index" class="min-h-24 border-b border-r border-surface bg-transparent p-2 text-left align-top" :class="{ 'opacity-35': !day.current }"><span class="text-xs">{{ day.date > 0 ? day.date : 30 + day.date }}</span><span v-for="event in day.events" :key="event" class="mt-2 block truncate rounded-md bg-emphasis px-2 py-1 text-xs text-primary">{{ event }}</span></button></div></section><section class="workspace-panel"><div class="panel-heading"><h2>{{ $t('calendar.upcoming') }}</h2></div><div v-for="item in upcoming" :key="item.time" class="flex gap-3 border-b border-surface py-4 last:border-0"><span class="mt-1 size-2 rounded-full" :style="{background:item.color}" /><div><strong class="block text-sm font-medium">{{ item.title }}</strong><small class="text-muted-color">{{ item.time }}</small></div></div></section></div>
  <Dialog v-model:visible="dialogOpen" modal :header="$t('calendar.new')" class="w-[420px] max-w-[90vw]"><div class="grid gap-4"><label class="grid gap-2 text-sm"><span>{{ $t('calendar.eventName') }}</span><InputText /></label><label class="grid gap-2 text-sm"><span>{{ $t('calendar.eventDate') }}</span><DatePicker show-time fluid /></label></div><template #footer><Button :label="$t('common.cancel')" text @click="dialogOpen = false" /><Button :label="$t('common.save')" @click="dialogOpen = false" /></template></Dialog>
</template>
