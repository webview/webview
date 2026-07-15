<script setup lang="ts">
import { computed, ref } from 'vue'
import Button from 'primevue/button'
import Column from 'primevue/column'
import DataTable from 'primevue/datatable'
import DatePicker from 'primevue/datepicker'
import BaseChart from '@/components/charts/BaseChart.vue'
import MetricCard from '@/components/common/MetricCard.vue'
import PageHeader from '@/components/common/PageHeader.vue'

const range = ref([new Date(2026, 6, 1), new Date(2026, 6, 15)])
const trend = computed(() => ({ grid: { left: 35, right: 12, top: 20, bottom: 25 }, tooltip: { trigger: 'axis' }, xAxis: { type: 'category', data: ['W1','W2','W3','W4','W5','W6'] }, yAxis: { type: 'value' }, series: [{ type: 'line', smooth: true, data: [120,168,151,219,238,286], lineStyle: { color: '#4f6bed', width: 3 }, areaStyle: { color: 'rgba(79,107,237,.12)' } }] }))
const channels = computed(() => ({ tooltip: { trigger: 'item' }, legend: { bottom: 0 }, series: [{ type: 'pie', radius: ['48%','72%'], data: [{value:45,name:'Direct'},{value:28,name:'Search'},{value:17,name:'Social'},{value:10,name:'Other'}] }] }))
const rows = [{ source: 'Direct', visits: '12,482', conversion: '8.4%', change: '+12.6%' }, { source: 'Search', visits: '8,310', conversion: '6.8%', change: '+8.2%' }, { source: 'Social', visits: '5,091', conversion: '4.3%', change: '-2.1%' }]
</script>

<template>
  <PageHeader :title="$t('analytics.title')" :description="$t('analytics.description')"><template #actions><DatePicker v-model="range" selection-mode="range" :manual-input="false" show-icon /><Button :label="$t('analytics.export')" icon="pi pi-download" outlined /></template></PageHeader>
  <div class="grid gap-3 sm:grid-cols-2 xl:grid-cols-4"><MetricCard :label="$t('analytics.visits')" value="25,883" helper="+12.6%" icon="pi pi-eye" /><MetricCard :label="$t('analytics.users')" value="8,429" helper="+8.4%" icon="pi pi-users" tone="success" /><MetricCard :label="$t('analytics.conversion')" value="7.2%" helper="+1.1%" icon="pi pi-bolt" tone="warning" /><MetricCard :label="$t('analytics.duration')" value="4m 18s" helper="+22s" icon="pi pi-clock" tone="violet" /></div>
  <div class="mt-3 grid gap-3 xl:grid-cols-[1.4fr_.8fr]"><section class="workspace-panel"><div class="panel-heading"><h2>{{ $t('analytics.trend') }}</h2></div><BaseChart :option="trend" :summary="$t('analytics.trendSummary')" /></section><section class="workspace-panel"><div class="panel-heading"><h2>{{ $t('analytics.channels') }}</h2></div><BaseChart :option="channels" :summary="$t('analytics.channelsSummary')" /></section></div>
  <section class="workspace-panel mt-3"><div class="panel-heading"><h2>{{ $t('analytics.sourceDetails') }}</h2></div><DataTable :value="rows"><Column field="source" :header="$t('analytics.source')" /><Column field="visits" :header="$t('analytics.visits')" /><Column field="conversion" :header="$t('analytics.conversion')" /><Column field="change" :header="$t('analytics.change')" /></DataTable></section>
</template>
