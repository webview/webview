<script setup lang="ts">
import Avatar from 'primevue/avatar'
import Button from 'primevue/button'
import InputText from 'primevue/inputtext'
import Select from 'primevue/select'
import Textarea from 'primevue/textarea'
import PageHeader from '@/components/common/PageHeader.vue'
</script>

<template>
  <PageHeader :title="$t('profile.title')" :description="$t('profile.description')">
    <template #actions><Button :label="$t('common.save')" icon="pi pi-check" /></template>
  </PageHeader>
  <div class="grid gap-3 xl:grid-cols-[280px_minmax(0,1fr)]">
    <section class="workspace-panel text-center">
      <Avatar :label="$t('user.initials')" shape="circle" class="mb-4 size-24 text-2xl" />
      <h2 class="m-0 text-lg font-medium">{{ $t('user.name') }}</h2>
      <p class="mt-1 text-sm text-muted-color">lin@northstar.app</p>
      <Button :label="$t('profile.changeAvatar')" outlined size="small" />
    </section>
    <section class="workspace-panel">
      <div class="panel-heading"><div><h2>{{ $t('profile.basic') }}</h2><p>{{ $t('profile.basicHelp') }}</p></div></div>
      <form class="grid gap-5 md:grid-cols-2">
        <label class="form-field"><span>{{ $t('profile.fullName') }}</span><InputText :model-value="$t('user.name')" /></label>
        <label class="form-field"><span>{{ $t('profile.email') }}</span><InputText model-value="lin@northstar.app" type="email" /></label>
        <label class="form-field"><span>{{ $t('profile.role') }}</span><Select :options="[$t('profile.designer'),$t('profile.manager')]" :model-value="$t('profile.designer')" /></label>
        <label class="form-field"><span>{{ $t('profile.timezone') }}</span><Select :options="['Asia/Shanghai (GMT+8)','America/Los_Angeles (GMT-7)']" model-value="Asia/Shanghai (GMT+8)" /></label>
        <label class="form-field md:col-span-2"><span>{{ $t('profile.bio') }}</span><Textarea rows="4" :placeholder="$t('profile.bioPlaceholder')" /></label>
      </form>
    </section>
  </div>
</template>
