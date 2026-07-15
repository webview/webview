<script setup lang="ts">
import { onMounted } from 'vue'
import Button from 'primevue/button'
import Column from 'primevue/column'
import DataTable from 'primevue/datatable'
import Tabs from 'primevue/tabs'; import TabList from 'primevue/tablist'; import Tab from 'primevue/tab'; import TabPanels from 'primevue/tabpanels'; import TabPanel from 'primevue/tabpanel'
import AsyncContent from '@/components/common/AsyncContent.vue'
import PageHeader from '@/components/common/PageHeader.vue'
import StatusBadge from '@/components/common/StatusBadge.vue'
import { useAsyncData } from '@/composables/useAsyncData'
import { getTasks } from '@/services/workspace'

const columns = ['todo', 'doing', 'review', 'done']
const { data: tasks, loading, error, load } = useAsyncData(getTasks)
onMounted(load)
</script>

<template>
  <PageHeader :title="$t('tasks.title')" :description="$t('tasks.description')"><template #actions><Button :label="$t('tasks.new')" icon="pi pi-plus" /></template></PageHeader>
  <AsyncContent :loading="loading" :error="error" :empty="!tasks?.length" @retry="load"><Tabs value="board" class="workspace-panel"><TabList><Tab value="board">{{ $t('tasks.board') }}</Tab><Tab value="list">{{ $t('tasks.list') }}</Tab></TabList><TabPanels><TabPanel value="board"><div class="grid gap-3 lg:grid-cols-4"><section v-for="column in columns" :key="column" class="rounded-xl bg-emphasis p-3"><div class="mb-3 flex items-center justify-between"><strong>{{ $t(`status.${column}`) }}</strong><span>{{ tasks?.filter(t=>t.status===column).length }}</span></div><article v-for="task in tasks?.filter(t=>t.status===column)" :key="task.id" class="mb-2 rounded-xl border border-surface bg-surface-0 p-3 dark:bg-surface-900"><StatusBadge :label="$t(`priority.${task.priority}`)" :severity="task.priority==='high'?'danger':'secondary'" /><h3 class="mb-1 mt-3 text-sm font-medium">{{ $t(task.titleKey) }}</h3><p class="m-0 text-xs text-muted-color">{{ $t(task.projectKey) }} · {{ $t(task.ownerKey) }}</p></article></section></div></TabPanel><TabPanel value="list"><DataTable :value="tasks"><Column :header="$t('tasks.task')"><template #body="{data:item}">{{ $t(item.titleKey) }}</template></Column><Column :header="$t('tasks.project')"><template #body="{data:item}">{{ $t(item.projectKey) }}</template></Column><Column :header="$t('tasks.owner')"><template #body="{data:item}">{{ $t(item.ownerKey) }}</template></Column><Column :header="$t('tasks.status')"><template #body="{data:item}"><StatusBadge :label="$t(`status.${item.status}`)" /></template></Column></DataTable></TabPanel></TabPanels></Tabs></AsyncContent>
</template>
