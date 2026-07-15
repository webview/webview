<script setup lang="ts">
import { storeToRefs } from 'pinia'
import { navigationGroups } from '@/config/navigation'
import { useAppStore } from '@/stores/app'
import { useUserStore } from '@/stores/user'

const appStore = useAppStore()
const { compactViewport, sidebarIsCollapsed } = storeToRefs(appStore)
const { currentUser } = storeToRefs(useUserStore())
</script>

<template>
  <aside
    class="app-sidebar"
    :class="{ 'is-collapsed': sidebarIsCollapsed }"
  >
    <div class="sidebar-brand flex h-16 items-center gap-2.5 px-3">
      <span class="grid size-9 shrink-0 place-items-center rounded-xl bg-primary text-primary-contrast shadow-sm shadow-primary/20">
        <i class="pi pi-sparkles text-sm" aria-hidden="true" />
      </span>
      <strong v-if="!sidebarIsCollapsed" class="min-w-0 flex-1 truncate text-[15px] tracking-tight">{{ $t('app.name') }}</strong>
      <button
        v-if="!compactViewport"
        v-tooltip.right="sidebarIsCollapsed ? $t('navigation.expand') : undefined"
        type="button"
        class="sidebar-icon-button"
        data-testid="sidebar-toggle"
        :aria-label="$t(sidebarIsCollapsed ? 'navigation.expand' : 'navigation.collapse')"
        @click="appStore.toggleSidebar"
      >
        <i :class="sidebarIsCollapsed ? 'pi pi-angle-double-right' : 'pi pi-angle-double-left'" aria-hidden="true" />
      </button>
    </div>

    <nav class="flex-1 overflow-y-auto px-2.5 pb-3" :aria-label="$t('navigation.main')">
      <section v-for="group in navigationGroups" :key="group.labelKey" class="mb-4">
        <p v-if="!sidebarIsCollapsed" class="px-2.5 pb-1.5 pt-2 text-[11px] font-medium text-muted-color">
          {{ $t(group.labelKey) }}
        </p>
        <div class="space-y-1">
          <RouterLink
            v-for="item in group.items"
            :key="item.labelKey"
            v-tooltip.right="sidebarIsCollapsed ? $t(item.labelKey) : undefined"
            :to="item.to"
            class="sidebar-link"
          >
            <i :class="item.icon" aria-hidden="true" />
            <span v-if="!sidebarIsCollapsed" class="min-w-0 flex-1 truncate">{{ $t(item.labelKey) }}</span>
            <span v-if="item.badge && !sidebarIsCollapsed" class="rounded-md bg-emphasis px-1.5 py-0.5 text-[10px] text-muted-color">{{ item.badge }}</span>
          </RouterLink>
        </div>
      </section>
    </nav>

    <button type="button" class="mx-2.5 flex items-center gap-2.5 rounded-xl p-2 text-left transition hover:bg-emphasis">
      <span class="grid size-9 shrink-0 place-items-center rounded-xl bg-primary text-sm font-medium text-primary-contrast">{{ $t(currentUser.initialsKey) }}</span>
      <span v-if="!sidebarIsCollapsed" class="grid min-w-0 flex-1">
        <strong class="truncate text-sm font-medium">{{ $t(currentUser.nameKey) }}</strong>
        <small class="truncate text-xs text-muted-color">{{ $t(currentUser.roleKey) }}</small>
      </span>
      <i v-if="!sidebarIsCollapsed" class="pi pi-angle-up text-xs text-muted-color" aria-hidden="true" />
    </button>
  </aside>
</template>
