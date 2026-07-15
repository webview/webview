<script setup lang="ts">
import Button from 'primevue/button'
import Column from 'primevue/column'
import DataTable from 'primevue/datatable'
import FileUpload from 'primevue/fileupload'
import Tree from 'primevue/tree'
import PageHeader from '@/components/common/PageHeader.vue'

const folders = [{ key: '0', label: 'Northstar 2.0', icon: 'pi pi-folder', children: [{ key: '0-0', label: 'Design', icon: 'pi pi-folder' }, { key: '0-1', label: 'Product Docs', icon: 'pi pi-folder' }] }, { key: '1', label: 'Brand Resources', icon: 'pi pi-folder' }]
const files = [{ name: 'Product-guidelines.pdf', type: 'PDF', size: '8.4 MB', modified: 'Today 10:24' }, { name: 'Dashboard-v3.fig', type: 'Figma', size: '24.6 MB', modified: 'Yesterday 18:42' }, { name: 'Release-checklist.xlsx', type: 'Excel', size: '1.2 MB', modified: 'Jul 13' }, { name: 'Brand-illustrations.zip', type: 'Archive', size: '68.3 MB', modified: 'Jul 11' }]
</script>

<template>
  <PageHeader :title="$t('files.title')" :description="$t('files.description')"><template #actions><FileUpload mode="basic" auto custom-upload :choose-label="$t('files.upload')" choose-icon="pi pi-upload" /><Button :label="$t('files.newFolder')" icon="pi pi-folder-plus" outlined /></template></PageHeader>
  <div class="grid gap-3 lg:grid-cols-[260px_minmax(0,1fr)]"><section class="workspace-panel"><div class="panel-heading"><h2>{{ $t('files.folders') }}</h2><Button icon="pi pi-plus" text rounded /></div><Tree :value="folders" class="border-0 bg-transparent p-0" /></section><section class="workspace-panel"><div class="mb-4 flex items-center justify-between"><strong>{{ $t('files.recent') }}</strong><Button icon="pi pi-ellipsis-h" text rounded /></div><DataTable :value="files" striped-rows><Column :header="$t('files.name')"><template #body="{data}"><div class="flex items-center gap-3"><span class="grid size-9 place-items-center rounded-xl bg-emphasis text-primary"><i class="pi pi-file" /></span><strong class="font-medium">{{ data.name }}</strong></div></template></Column><Column field="type" :header="$t('files.type')" /><Column field="size" :header="$t('files.size')" /><Column field="modified" :header="$t('files.modified')" /></DataTable></section></div>
</template>
