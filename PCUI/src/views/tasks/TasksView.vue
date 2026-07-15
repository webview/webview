<script setup lang="ts">
import Button from 'primevue/button'
import Column from 'primevue/column'
import DataTable from 'primevue/datatable'
import Tabs from 'primevue/tabs'
import TabList from 'primevue/tablist'
import Tab from 'primevue/tab'
import TabPanels from 'primevue/tabpanels'
import TabPanel from 'primevue/tabpanel'
import PageHeader from '@/components/common/PageHeader.vue'
import StatusBadge from '@/components/common/StatusBadge.vue'

const tasks = [
  { title: 'Complete prototype review', project: 'Northstar 2.0', priority: 'high', status: 'doing', owner: 'Lin Qingyue' },
  { title: 'Document interaction states', project: 'Design System', priority: 'medium', status: 'todo', owner: 'Zhou Yu' },
  { title: 'Analyze quarterly growth', project: 'Growth Experiments', priority: 'high', status: 'review', owner: 'Shen Xing' },
  { title: 'Archive brand assets', project: 'Brand Resources', priority: 'low', status: 'done', owner: 'Gu Yan' },
]
const columns = ['todo', 'doing', 'review', 'done']
</script>

<template>
  <PageHeader :title="$t('tasks.title')" :description="$t('tasks.description')"><template #actions><Button :label="$t('tasks.new')" icon="pi pi-plus" /></template></PageHeader>
  <Tabs value="board" class="workspace-panel">
    <TabList><Tab value="board">{{ $t('tasks.board') }}</Tab><Tab value="list">{{ $t('tasks.list') }}</Tab></TabList>
    <TabPanels>
      <TabPanel value="board"><div class="grid gap-3 lg:grid-cols-4"><section v-for="column in columns" :key="column" class="rounded-xl bg-emphasis p-3"><div class="mb-3 flex items-center justify-between"><strong class="text-sm">{{ $t(`status.${column}`) }}</strong><span class="text-xs text-muted-color">{{ tasks.filter(t => t.status === column).length }}</span></div><article v-for="task in tasks.filter(t => t.status === column)" :key="task.title" class="mb-2 rounded-xl border border-surface bg-surface-0 p-3 last:mb-0 dark:bg-surface-900"><StatusBadge :label="$t(`priority.${task.priority}`)" :severity="task.priority === 'high' ? 'danger' : 'secondary'" /><h3 class="mb-1 mt-3 text-sm font-medium">{{ task.title }}</h3><p class="m-0 text-xs text-muted-color">{{ task.project }} · {{ task.owner }}</p></article></section></div></TabPanel>
      <TabPanel value="list"><DataTable :value="tasks"><Column field="title" :header="$t('tasks.task')" /><Column field="project" :header="$t('tasks.project')" /><Column field="owner" :header="$t('tasks.owner')" /><Column :header="$t('tasks.status')"><template #body="{data}"><StatusBadge :label="$t(`status.${data.status}`)" /></template></Column></DataTable></TabPanel>
    </TabPanels>
  </Tabs>
</template>
