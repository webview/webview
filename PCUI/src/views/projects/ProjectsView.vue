<script setup lang="ts">
import { computed, onMounted, ref } from 'vue'
import { useI18n } from 'vue-i18n'
import Button from 'primevue/button'
import Column from 'primevue/column'
import DataTable from 'primevue/datatable'
import Drawer from 'primevue/drawer'
import InputText from 'primevue/inputtext'
import ProgressBar from 'primevue/progressbar'
import Select from 'primevue/select'
import AsyncContent from '@/components/common/AsyncContent.vue'
import PageHeader from '@/components/common/PageHeader.vue'
import StatusBadge from '@/components/common/StatusBadge.vue'
import { useAsyncData } from '@/composables/useAsyncData'
import { filterProjects, getProjects, type ProjectRecord } from '@/services/workspace'

const { t } = useI18n()
const view = ref<'table' | 'grid'>('table')
const query = ref('')
const status = ref('all')
const selected = ref<ProjectRecord>()
const drawerOpen = ref(false)
const { data, loading, error, load } = useAsyncData(getProjects)
const filtered = computed(() => filterProjects(data.value || [], '', status.value).filter((item) => t(item.nameKey).toLowerCase().includes(query.value.trim().toLowerCase())))
const openProject = (project: ProjectRecord) => { selected.value = project; drawerOpen.value = true }
onMounted(load)
</script>

<template>
  <PageHeader :title="$t('projects.title')" :description="$t('projects.description')"><template #actions><Button :label="$t('projects.new')" icon="pi pi-plus" /></template></PageHeader>
  <AsyncContent :loading="loading" :error="error" :empty="!data?.length" @retry="load">
    <section class="workspace-panel">
      <div class="mb-5 flex flex-wrap items-center gap-2"><span class="p-input-icon-left min-w-56 flex-1"><i class="pi pi-search" /><InputText v-model="query" class="w-full" :placeholder="$t('projects.search')" /></span><Select v-model="status" :options="[{label:$t('common.all'),value:'all'},{label:$t('status.active'),value:'active'},{label:$t('status.review'),value:'review'}]" option-label="label" option-value="value" /><div class="flex rounded-xl bg-emphasis p-1"><Button icon="pi pi-list" text size="small" :severity="view === 'table' ? 'primary' : 'secondary'" @click="view='table'" /><Button icon="pi pi-th-large" text size="small" :severity="view === 'grid' ? 'primary' : 'secondary'" @click="view='grid'" /></div></div>
      <DataTable v-if="view === 'table'" :value="filtered" striped-rows paginator :rows="6" @row-click="openProject($event.data)"><Column :header="$t('projects.name')"><template #body="{data:item}">{{ $t(item.nameKey) }}</template></Column><Column :header="$t('projects.owner')"><template #body="{data:item}">{{ $t(item.ownerKey) }}</template></Column><Column :header="$t('projects.status')"><template #body="{data:item}"><StatusBadge :label="$t(`status.${item.status}`)" :severity="item.status === 'active' ? 'success' : 'secondary'" /></template></Column><Column :header="$t('projects.progress')"><template #body="{data:item}"><div class="flex items-center gap-3"><ProgressBar :value="item.progress" :show-value="false" class="h-1.5 w-28" /><span>{{ item.progress }}%</span></div></template></Column><Column field="due" :header="$t('projects.due')" /></DataTable>
      <div v-else class="grid gap-3 md:grid-cols-2 xl:grid-cols-3"><button v-for="project in filtered" :key="project.id" class="rounded-2xl border border-surface bg-transparent p-5 text-left transition hover:-translate-y-0.5 hover:border-primary/40 hover:shadow-lg" @click="openProject(project)"><div class="mb-6 flex items-start justify-between"><span class="grid size-10 place-items-center rounded-xl bg-emphasis text-primary"><i class="pi pi-folder" /></span><StatusBadge :label="$t(`status.${project.status}`)" /></div><strong>{{ $t(project.nameKey) }}</strong><p class="text-sm text-muted-color">{{ $t(project.ownerKey) }} · {{ project.due }}</p><ProgressBar :value="project.progress" :show-value="false" class="h-1.5" /></button></div>
    </section>
  </AsyncContent>
  <Drawer v-model:visible="drawerOpen" position="right" class="w-full sm:w-[420px]" :header="selected ? $t(selected.nameKey) : ''"><p class="text-muted-color">{{ $t('projects.drawerDescription') }}</p></Drawer>
</template>
