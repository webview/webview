<script setup lang="ts">
import { computed, onMounted, ref } from 'vue'
import { useI18n } from 'vue-i18n'
import Button from 'primevue/button'; import Column from 'primevue/column'; import DataTable from 'primevue/datatable'; import FileUpload from 'primevue/fileupload'; import ProgressBar from 'primevue/progressbar'; import Tree from 'primevue/tree'
import AsyncContent from '@/components/common/AsyncContent.vue'
import PageHeader from '@/components/common/PageHeader.vue'
import { useAsyncData } from '@/composables/useAsyncData'
import { getFiles } from '@/services/workspace'

const { t } = useI18n()
const folders = computed(() => [{ key: '0', label: t('samples.projects.northstar'), icon: 'pi pi-folder', children: [{ key: '0-0', label: t('files.designFolder'), icon: 'pi pi-folder' }, { key: '0-1', label: t('files.docsFolder'), icon: 'pi pi-folder' }] }])
const uploadProgress = ref(0)
const { data: files, loading, error, load } = useAsyncData(getFiles)
const upload = () => { uploadProgress.value = 35; window.setTimeout(() => { uploadProgress.value = 100 }, 220); window.setTimeout(() => { uploadProgress.value = 0 }, 900) }
onMounted(load)
</script>

<template>
  <PageHeader :title="$t('files.title')" :description="$t('files.description')"><template #actions><FileUpload mode="basic" auto custom-upload :choose-label="$t('files.upload')" choose-icon="pi pi-upload" @uploader="upload" /><Button :label="$t('files.newFolder')" icon="pi pi-folder-plus" outlined /></template></PageHeader>
  <ProgressBar v-if="uploadProgress" :value="uploadProgress" class="mb-3 h-2" />
  <AsyncContent :loading="loading" :error="error" :empty="!files?.length" @retry="load"><div class="grid gap-3 lg:grid-cols-[260px_minmax(0,1fr)]"><section class="workspace-panel"><div class="panel-heading"><h2>{{ $t('files.folders') }}</h2><Button icon="pi pi-plus" text rounded /></div><Tree :value="folders" class="border-0 bg-transparent p-0" /></section><section class="workspace-panel"><div class="mb-4 flex items-center justify-between"><strong>{{ $t('files.recent') }}</strong><Button icon="pi pi-ellipsis-h" text rounded /></div><DataTable :value="files" striped-rows><Column :header="$t('files.name')"><template #body="{data:item}"><div class="flex items-center gap-3"><span class="grid size-9 place-items-center rounded-xl bg-emphasis text-primary"><i class="pi pi-file" /></span><strong>{{ $t(item.nameKey) }}</strong></div></template></Column><Column field="type" :header="$t('files.type')" /><Column field="size" :header="$t('files.size')" /><Column :header="$t('files.modified')"><template #body="{data:item}">{{ $t(item.modifiedKey) }}</template></Column><Column><template #body><Button icon="pi pi-ellipsis-v" text rounded :aria-label="$t('files.actions')" /></template></Column></DataTable></section></div></AsyncContent>
</template>
