<script setup lang="ts">
import { ref } from 'vue'
import Avatar from 'primevue/avatar'
import AvatarGroup from 'primevue/avatargroup'
import Button from 'primevue/button'
import Column from 'primevue/column'
import DataTable from 'primevue/datatable'
import Dialog from 'primevue/dialog'
import InputText from 'primevue/inputtext'
import Select from 'primevue/select'
import PageHeader from '@/components/common/PageHeader.vue'
import StatusBadge from '@/components/common/StatusBadge.vue'

const inviteOpen = ref(false)
const members = [{ name: '林清越', initials: '林', role: 'owner', team: '产品设计', status: 'online', joined: '2025-03-18' }, { name: '周屿', initials: '周', role: 'admin', team: '产品设计', status: 'online', joined: '2025-06-02' }, { name: '沈星', initials: '沈', role: 'member', team: '数据增长', status: 'busy', joined: '2025-09-14' }, { name: '顾言', initials: '顾', role: 'member', team: '产品研发', status: 'offline', joined: '2026-01-08' }]
</script>

<template>
  <PageHeader :title="$t('team.title')" :description="$t('team.description')"><template #actions><AvatarGroup><Avatar v-for="item in members.slice(0,3)" :key="item.name" :label="item.initials" shape="circle" /></AvatarGroup><Button :label="$t('team.invite')" icon="pi pi-user-plus" @click="inviteOpen = true" /></template></PageHeader>
  <section class="workspace-panel"><div class="mb-4 flex flex-wrap items-center gap-2"><span class="p-input-icon-left flex-1"><i class="pi pi-search" /><InputText class="w-full" :placeholder="$t('team.search')" /></span><Select :options="[$t('common.all'),$t('team.product'),$t('team.engineering')]" :placeholder="$t('team.filter')" /></div><DataTable :value="members" paginator :rows="8"><Column :header="$t('team.member')"><template #body="{data}"><div class="flex items-center gap-3"><Avatar :label="data.initials" shape="circle" /><strong class="font-medium">{{ data.name }}</strong></div></template></Column><Column :header="$t('team.role')"><template #body="{data}">{{ $t(`roles.${data.role}`) }}</template></Column><Column field="team" :header="$t('team.group')" /><Column :header="$t('team.status')"><template #body="{data}"><StatusBadge :label="$t(`presence.${data.status}`)" :severity="data.status === 'online' ? 'success' : 'secondary'" /></template></Column><Column field="joined" :header="$t('team.joined')" /></DataTable></section>
  <Dialog v-model:visible="inviteOpen" modal :header="$t('team.invite')" class="w-[440px] max-w-[90vw]"><div class="grid gap-4"><label class="grid gap-2 text-sm"><span>{{ $t('team.email') }}</span><InputText type="email" placeholder="name@company.com" /></label><label class="grid gap-2 text-sm"><span>{{ $t('team.role') }}</span><Select :options="[$t('roles.admin'),$t('roles.member')]" /></label></div><template #footer><Button :label="$t('common.cancel')" text @click="inviteOpen=false" /><Button :label="$t('team.sendInvite')" @click="inviteOpen=false" /></template></Dialog>
</template>
