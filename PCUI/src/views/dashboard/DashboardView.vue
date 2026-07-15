<script setup lang="ts">
import { computed, onMounted, ref } from 'vue'
import { useI18n } from 'vue-i18n'
import Button from 'primevue/button'
import ProgressBar from 'primevue/progressbar'
import AsyncContent from '@/components/common/AsyncContent.vue'
import BaseChart from '@/components/charts/BaseChart.vue'
import MetricCard from '@/components/common/MetricCard.vue'
import PageHeader from '@/components/common/PageHeader.vue'
import StatusBadge from '@/components/common/StatusBadge.vue'
import { getDashboardData } from '@/services/dashboard'
import type { DashboardData } from '@/types/dashboard'

const { t } = useI18n()
const data = ref<DashboardData>()
const loading = ref(true)
const error = ref<string>()

const load = async () => {
  loading.value = true
  error.value = undefined
  try { data.value = await getDashboardData() }
  catch { error.value = t('common.loadFailed') }
  finally { loading.value = false }
}

const chartOption = computed(() => ({
  grid: { left: 30, right: 16, top: 24, bottom: 28 },
  tooltip: { trigger: 'axis' },
  xAxis: { type: 'category', boundaryGap: false, data: t('dashboard.weekdays').split(',') },
  yAxis: { type: 'value', splitLine: { lineStyle: { color: 'rgba(148,163,184,.18)' } } },
  series: [{ type: 'line', smooth: true, symbolSize: 7, data: data.value?.progress || [], lineStyle: { width: 3, color: '#4f6bed' }, itemStyle: { color: '#4f6bed' }, areaStyle: { color: 'rgba(79,107,237,.12)' } }],
}))

onMounted(load)
</script>

<template>
  <PageHeader :eyebrow="$t('dashboard.date')" :title="$t('dashboard.greeting')" :description="$t('dashboard.description')">
    <template #actions><Button :label="$t('dashboard.newProject')" icon="pi pi-plus" /></template>
  </PageHeader>
  <AsyncContent :loading="loading" :error="error" :empty="!data" @retry="load">
    <div v-if="data" class="grid gap-3 sm:grid-cols-2 xl:grid-cols-4">
      <MetricCard v-for="metric in data.metrics" :key="metric.id" :label="$t(metric.labelKey)" :value="metric.value" :helper="$t(metric.helperKey)" :icon="metric.icon" :tone="metric.tone" />
    </div>
    <div v-if="data" class="mt-3 grid gap-3 xl:grid-cols-[minmax(0,1.45fr)_minmax(300px,.8fr)]">
      <section class="workspace-panel">
        <div class="panel-heading"><div><h2>{{ $t('dashboard.progress') }}</h2><p>{{ $t('dashboard.progressHelp') }}</p></div><Button icon="pi pi-ellipsis-h" text rounded aria-label="More" /></div>
        <BaseChart :option="chartOption" :summary="$t('dashboard.progressSummary')" height="250px" />
      </section>
      <section class="workspace-panel">
        <div class="panel-heading"><div><h2>{{ $t('dashboard.todayTasks') }}</h2><p>{{ $t('dashboard.taskCount') }}</p></div><Button :label="$t('common.viewAll')" text size="small" /></div>
        <div v-for="task in data.tasks" :key="task.id" class="flex items-center gap-3 border-b border-surface py-4 last:border-0">
          <span class="grid size-6 place-items-center rounded-lg border" :class="task.completed ? 'border-primary bg-primary text-primary-contrast' : 'border-surface'"><i v-if="task.completed" class="pi pi-check text-xs" /></span>
          <div class="min-w-0 flex-1"><strong class="block truncate text-sm font-medium" :class="{ 'text-muted-color line-through': task.completed }">{{ task.title }}</strong><small class="text-muted-color">{{ task.project }}</small></div><time class="text-xs text-muted-color">{{ task.time }}</time>
        </div>
      </section>
    </div>
    <section v-if="data" class="workspace-panel mt-3">
      <div class="panel-heading"><div><h2>{{ $t('dashboard.recentProjects') }}</h2><p>{{ $t('dashboard.recentProjectsHelp') }}</p></div><Button :label="$t('common.viewAll')" text size="small" /></div>
      <div class="grid gap-3 md:grid-cols-3">
        <article v-for="project in data.projects" :key="project.id" class="rounded-xl bg-emphasis p-4">
          <div class="mb-4 flex items-center justify-between"><strong>{{ project.name }}</strong><StatusBadge :label="$t(`status.${project.status}`)" :severity="project.status === 'active' ? 'success' : 'secondary'" /></div>
          <ProgressBar :value="project.progress" :show-value="false" class="h-1.5" /><p class="mb-0 mt-2 text-xs text-muted-color">{{ project.progress }}% {{ $t('common.completed') }}</p>
        </article>
      </div>
    </section>
  </AsyncContent>
</template>
