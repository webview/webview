<script setup lang="ts">
import { storeToRefs } from 'pinia'
import Button from 'primevue/button'
import Select from 'primevue/select'
import ToggleSwitch from 'primevue/toggleswitch'
import { ref } from 'vue'
import PageHeader from '@/components/common/PageHeader.vue'
import { useAppStore } from '@/stores/app'

const store = useAppStore()
const { theme, locale } = storeToRefs(store)
const notifications = ref(true)
const sounds = ref(false)
</script>

<template>
  <PageHeader :title="$t('settings.title')" :description="$t('settings.description')" />
  <div class="grid gap-3 xl:grid-cols-[220px_minmax(0,1fr)]"><nav class="workspace-panel self-start"><a v-for="item in ['appearance','notifications','shortcuts']" :key="item" href="#" class="mb-1 flex items-center gap-2 rounded-xl px-3 py-2.5 text-sm text-color no-underline first:bg-emphasis first:text-primary"><i :class="item === 'appearance' ? 'pi pi-palette' : item === 'notifications' ? 'pi pi-bell' : 'pi pi-bolt'" />{{ $t(`settings.${item}`) }}</a></nav><div class="space-y-3"><section class="workspace-panel"><div class="panel-heading"><div><h2>{{ $t('settings.appearance') }}</h2><p>{{ $t('settings.appearanceHelp') }}</p></div></div><div class="setting-row"><div><strong>{{ $t('settings.theme') }}</strong><p>{{ $t('settings.themeHelp') }}</p></div><Select :model-value="theme" :options="[{label:$t('settings.light'),value:'light'},{label:$t('settings.dark'),value:'dark'},{label:$t('settings.system'),value:'system'}]" option-label="label" option-value="value" @update:model-value="store.setTheme($event)" /></div><div class="setting-row"><div><strong>{{ $t('settings.language') }}</strong><p>{{ $t('settings.languageHelp') }}</p></div><Select :model-value="locale" :options="[{label:'简体中文',value:'zh-CN'},{label:'English',value:'en-US'}]" option-label="label" option-value="value" @update:model-value="store.setLocale($event)" /></div></section><section class="workspace-panel"><div class="panel-heading"><div><h2>{{ $t('settings.notifications') }}</h2><p>{{ $t('settings.notificationsHelp') }}</p></div></div><div class="setting-row"><div><strong>{{ $t('settings.desktopNotifications') }}</strong><p>{{ $t('settings.desktopNotificationsHelp') }}</p></div><ToggleSwitch v-model="notifications" /></div><div class="setting-row"><div><strong>{{ $t('settings.sounds') }}</strong><p>{{ $t('settings.soundsHelp') }}</p></div><ToggleSwitch v-model="sounds" /></div></section><section class="workspace-panel"><div class="panel-heading"><h2>{{ $t('settings.shortcuts') }}</h2></div><div class="setting-row"><span>{{ $t('settings.quickSearch') }}</span><kbd>Ctrl K</kbd></div><div class="setting-row"><span>{{ $t('settings.newTask') }}</span><kbd>Ctrl Shift T</kbd></div><Button :label="$t('settings.customize')" outlined size="small" class="mt-4" /></section></div></div>
</template>
