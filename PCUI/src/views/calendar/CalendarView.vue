<script setup lang="ts">
import { computed, ref } from 'vue'
import { useI18n } from 'vue-i18n'
import Button from 'primevue/button'; import DatePicker from 'primevue/datepicker'; import Dialog from 'primevue/dialog'; import InputText from 'primevue/inputtext'
import PageHeader from '@/components/common/PageHeader.vue'

const { locale } = useI18n()
const dialogOpen = ref(false)
const monthOffset = ref(0)
const eventTitle = ref('')
const eventDate = ref<Date>()
const baseMonth = new Date(2026, 6, 1)
const monthDate = computed(() => new Date(baseMonth.getFullYear(), baseMonth.getMonth() + monthOffset.value, 1))
const monthLabel = computed(() => new Intl.DateTimeFormat(locale.value, { year: 'numeric', month: 'long' }).format(monthDate.value))
const days = computed(() => Array.from({ length: 35 }, (_, index) => ({ date: index - 1, current: index >= 2 && index < 33, eventKeys: index === 5 ? ['calendar.events.review'] : index === 12 ? ['calendar.events.sync','calendar.events.release'] : index === 22 ? ['calendar.events.interview'] : [] })))
const upcoming = ref([{ time: '10:00', titleKey: 'calendar.events.prototype', color: '#4f6bed' }, { time: '14:30', titleKey: 'calendar.events.design', color: '#16a36a' }, { time: '16:30', titleKey: 'calendar.events.sync', color: '#e59a18' }])
const saveEvent = () => { if (eventTitle.value.trim()) upcoming.value.push({ time: eventDate.value?.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' }) || '09:00', titleKey: eventTitle.value.trim(), color: '#4f6bed' }); dialogOpen.value=false; eventTitle.value='' }
</script>

<template><PageHeader :title="$t('calendar.title')" :description="$t('calendar.description')"><template #actions><Button :label="$t('calendar.new')" icon="pi pi-plus" @click="dialogOpen=true" /></template></PageHeader><div class="grid gap-3 xl:grid-cols-[minmax(0,1fr)_300px]"><section class="workspace-panel"><div class="mb-4 flex items-center justify-between"><div><Button icon="pi pi-angle-left" text rounded @click="monthOffset--" /><Button :label="$t('calendar.today')" text @click="monthOffset=0" /><Button icon="pi pi-angle-right" text rounded @click="monthOffset++" /></div><strong>{{ monthLabel }}</strong><Button icon="pi pi-ellipsis-h" text rounded /></div><div class="grid grid-cols-7 border-l border-t border-surface"><div v-for="day in $t('calendar.weekdays').split(',')" :key="day" class="border-b border-r border-surface p-2 text-center text-xs text-muted-color">{{ day }}</div><button v-for="(day,index) in days" :key="index" class="min-h-24 border-b border-r border-surface bg-transparent p-2 text-left" :class="{'opacity-35':!day.current}"><span class="text-xs">{{ day.date>0?day.date:30+day.date }}</span><span v-for="key in day.eventKeys" :key="key" class="mt-2 block truncate rounded-md bg-emphasis px-2 py-1 text-xs text-primary">{{ $t(key) }}</span></button></div></section><section class="workspace-panel"><div class="panel-heading"><h2>{{ $t('calendar.upcoming') }}</h2></div><div v-for="item in upcoming" :key="item.time+item.titleKey" class="flex gap-3 border-b border-surface py-4 last:border-0"><span class="mt-1 size-2 rounded-full" :style="{background:item.color}" /><div><strong class="block text-sm font-medium">{{ item.titleKey.includes('.') ? $t(item.titleKey) : item.titleKey }}</strong><small class="text-muted-color">{{ item.time }}</small></div></div></section></div><Dialog v-model:visible="dialogOpen" modal :header="$t('calendar.new')" class="w-[420px] max-w-[90vw]"><div class="grid gap-4"><label class="form-field"><span>{{ $t('calendar.eventName') }}</span><InputText v-model="eventTitle" /></label><label class="form-field"><span>{{ $t('calendar.eventDate') }}</span><DatePicker v-model="eventDate" show-time fluid /></label></div><template #footer><Button :label="$t('common.cancel')" text @click="dialogOpen=false" /><Button :label="$t('common.save')" :disabled="!eventTitle.trim()" @click="saveEvent" /></template></Dialog></template>
