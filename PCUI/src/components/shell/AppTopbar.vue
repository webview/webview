<script setup lang="ts">
import { computed } from 'vue'
import { useI18n } from 'vue-i18n'
import { useRoute } from 'vue-router'
import { storeToRefs } from 'pinia'
import Button from 'primevue/button'
import Menu from 'primevue/menu'
import type { MenuItem } from 'primevue/menuitem'
import { windowBridge } from '@/bridge/windowBridge'
import { useAppStore, type AppLocale, type ThemeMode } from '@/stores/app'
import { useNotificationStore } from '@/stores/notifications'
import { useUserStore } from '@/stores/user'
import WindowControls from './WindowControls.vue'

const route = useRoute()
const { t } = useI18n()
const store = useAppStore()
const { locale, theme } = storeToRefs(store)
const notificationStore = useNotificationStore()
const { unreadCount } = storeToRefs(notificationStore)
const { currentUser } = storeToRefs(useUserStore())
const titleKey = computed(() => route.meta.titleKey || 'navigation.dashboard')
const languageMenu = computed<MenuItem[]>(() => [
  { label: t('languageNames.chinese'), icon: locale.value === 'zh-CN' ? 'pi pi-check' : '', command: () => store.setLocale('zh-CN' as AppLocale) },
  { label: t('languageNames.english'), icon: locale.value === 'en-US' ? 'pi pi-check' : '', command: () => store.setLocale('en-US' as AppLocale) },
])
const themeMenu = computed<MenuItem[]>(() => [
  { label: t('settings.light'), icon: theme.value === 'light' ? 'pi pi-check' : 'pi pi-sun', command: () => store.setTheme('light' as ThemeMode) },
  { label: t('settings.dark'), icon: theme.value === 'dark' ? 'pi pi-check' : 'pi pi-moon', command: () => store.setTheme('dark' as ThemeMode) },
  { label: t('settings.system'), icon: theme.value === 'system' ? 'pi pi-check' : 'pi pi-desktop', command: () => store.setTheme('system' as ThemeMode) },
])
const notificationMenu = computed<MenuItem[]>(() => [
  ...notificationStore.items.map((item) => ({ label: t(item.titleKey), icon: item.read ? 'pi pi-check' : 'pi pi-circle-fill' })),
  { separator: true },
  { label: t('notifications.markAllRead'), icon: 'pi pi-check-circle', command: notificationStore.markAllRead },
])
</script>

<template>
  <header class="app-topbar">
    <div class="flex min-w-0 items-center gap-2 px-5 text-sm">
      <span class="hidden text-muted-color md:inline">{{ $t('app.workspace') }}</span>
      <i class="pi pi-angle-right hidden text-[10px] text-muted-color md:inline" aria-hidden="true" />
      <strong class="truncate font-medium">{{ $t(titleKey) }}</strong>
    </div>
    <div data-webview-drag-region class="h-full min-w-8 flex-1" @dblclick="windowBridge?.toggleMaximize" />
    <div class="flex h-full items-center gap-1">
      <button type="button" class="top-search" :aria-label="$t('actions.search')">
        <i class="pi pi-search" aria-hidden="true" />
        <span>{{ $t('actions.search') }}</span>
        <kbd>Ctrl K</kbd>
      </button>
      <Button v-tooltip.bottom="$t('actions.quickCreate')" icon="pi pi-plus" text rounded :aria-label="$t('actions.quickCreate')" as="router-link" :to="{ name: 'projects' }" />
      <Button v-tooltip.bottom="$t('actions.language')" icon="pi pi-language" text rounded :aria-label="$t('actions.language')" @click="($refs.language as InstanceType<typeof Menu>).toggle($event)" />
      <Menu ref="language" :model="languageMenu" popup />
      <Button v-tooltip.bottom="$t('actions.theme')" :icon="theme === 'dark' ? 'pi pi-moon' : 'pi pi-sun'" text rounded :aria-label="$t('actions.theme')" @click="($refs.themes as InstanceType<typeof Menu>).toggle($event)" />
      <Menu ref="themes" :model="themeMenu" popup />
      <span class="relative"><Button v-tooltip.bottom="$t('navigation.messages')" icon="pi pi-bell" text rounded :aria-label="$t('navigation.messages')" @click="($refs.notifications as InstanceType<typeof Menu>).toggle($event)" /><span v-if="unreadCount" class="absolute right-1 top-1 grid size-3.5 place-items-center rounded-full bg-red-500 text-[9px] text-white">{{ unreadCount }}</span></span>
      <Menu ref="notifications" :model="notificationMenu" popup />
      <Button v-tooltip.bottom="$t(currentUser.nameKey)" :label="$t(currentUser.initialsKey)" text rounded :aria-label="$t('actions.account')" as="router-link" :to="{ name: 'profile' }" />
      <WindowControls />
    </div>
  </header>
</template>
