<script setup lang="ts">
import { ref } from 'vue'
import Button from 'primevue/button'
import Column from 'primevue/column'
import DataTable from 'primevue/datatable'
import Drawer from 'primevue/drawer'
import InputText from 'primevue/inputtext'
import ProgressBar from 'primevue/progressbar'
import Select from 'primevue/select'
import PageHeader from '@/components/common/PageHeader.vue'
import StatusBadge from '@/components/common/StatusBadge.vue'

const view = ref<'table' | 'grid'>('table')
const selected = ref<Record<string, unknown>>()
const drawerOpen = ref(false)
const status = ref('all')
const projects = [
  { name: 'Northstar 2.0', owner: 'Lin Qingyue', status: 'active', progress: 72, due: '2026-08-18' },
  { name: 'Mobile Experience', owner: 'Gu Yan', status: 'review', progress: 48, due: '2026-08-28' },
  { name: 'Brand Resource Hub', owner: 'Zhou Yu', status: 'planned', progress: 24, due: '2026-09-12' },
  { name: 'Growth Experiments', owner: 'Shen Xing', status: 'active', progress: 61, due: '2026-09-30' },
]
const openProject = (project: Record<string, unknown>) => { selected.value = project; drawerOpen.value = true }
</script>

<template>
  <PageHeader :title="$t('projects.title')" :description="$t('projects.description')"><template #actions><Button :label="$t('projects.new')" icon="pi pi-plus" /></template></PageHeader>
  <section class="workspace-panel">
    <div class="mb-5 flex flex-wrap items-center gap-2">
      <span class="p-input-icon-left min-w-56 flex-1"><i class="pi pi-search" /><InputText class="w-full" :placeholder="$t('projects.search')" /></span>
      <Select v-model="status" :options="[{label:$t('common.all'),value:'all'},{label:$t('status.active'),value:'active'},{label:$t('status.review'),value:'review'}]" option-label="label" option-value="value" />
      <div class="flex rounded-xl bg-emphasis p-1"><Button icon="pi pi-list" text size="small" :severity="view === 'table' ? 'primary' : 'secondary'" @click="view = 'table'" /><Button icon="pi pi-th-large" text size="small" :severity="view === 'grid' ? 'primary' : 'secondary'" @click="view = 'grid'" /></div>
    </div>
    <DataTable v-if="view === 'table'" :value="projects" striped-rows paginator :rows="6" @row-click="openProject($event.data)">
      <Column field="name" :header="$t('projects.name')" /><Column field="owner" :header="$t('projects.owner')" /><Column :header="$t('projects.status')"><template #body="{data}"><StatusBadge :label="$t(`status.${data.status}`)" :severity="data.status === 'active' ? 'success' : 'secondary'" /></template></Column>
      <Column :header="$t('projects.progress')"><template #body="{data}"><div class="flex items-center gap-3"><ProgressBar :value="data.progress" :show-value="false" class="h-1.5 w-28" /><span class="text-xs text-muted-color">{{ data.progress }}%</span></div></template></Column><Column field="due" :header="$t('projects.due')" />
    </DataTable>
    <div v-else class="grid gap-3 md:grid-cols-2 xl:grid-cols-3"><button v-for="project in projects" :key="project.name" class="rounded-2xl border border-surface bg-transparent p-5 text-left transition hover:-translate-y-0.5 hover:border-primary/40 hover:shadow-lg" @click="openProject(project)"><div class="mb-6 flex items-start justify-between"><span class="grid size-10 place-items-center rounded-xl bg-emphasis text-primary"><i class="pi pi-folder" /></span><StatusBadge :label="$t(`status.${project.status}`)" /></div><strong>{{ project.name }}</strong><p class="text-sm text-muted-color">{{ project.owner }} · {{ project.due }}</p><ProgressBar :value="project.progress" :show-value="false" class="h-1.5" /></button></div>
  </section>
  <Drawer v-model:visible="drawerOpen" position="right" class="w-full sm:w-[420px]" :header="String(selected?.name || '')"><p class="text-muted-color">{{ $t('projects.drawerDescription') }}</p></Drawer>
</template>
