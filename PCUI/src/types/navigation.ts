import type { RouteLocationRaw } from 'vue-router'

export interface NavigationItem {
  labelKey: string
  icon: string
  to: RouteLocationRaw
  badge?: string
  permission?: string
}

export interface NavigationGroup {
  labelKey: string
  items: NavigationItem[]
}
