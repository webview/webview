import type { RouteRecordRaw } from 'vue-router'

const page = () => import('@/views/PlaceholderView.vue')

export const routes: RouteRecordRaw[] = [
  {
    path: '/',
    component: () => import('@/layouts/DefaultLayout.vue'),
    children: [
      { path: '', redirect: { name: 'dashboard' } },
      { path: 'dashboard', name: 'dashboard', component: page, meta: { titleKey: 'navigation.dashboard', icon: 'pi pi-objects-column' } },
      { path: 'projects', name: 'projects', component: page, meta: { titleKey: 'navigation.projects', icon: 'pi pi-folder' } },
      { path: 'tasks', name: 'tasks', component: page, meta: { titleKey: 'navigation.tasks', icon: 'pi pi-check-circle' } },
      { path: 'files', name: 'files', component: page, meta: { titleKey: 'navigation.files', icon: 'pi pi-file' } },
      { path: 'analytics', name: 'analytics', component: page, meta: { titleKey: 'navigation.analytics', icon: 'pi pi-chart-line' } },
      { path: 'team', name: 'team', component: page, meta: { titleKey: 'navigation.team', icon: 'pi pi-users' } },
      { path: 'messages', name: 'messages', component: page, meta: { titleKey: 'navigation.messages', icon: 'pi pi-inbox' } },
      { path: 'calendar', name: 'calendar', component: page, meta: { titleKey: 'navigation.calendar', icon: 'pi pi-calendar' } },
      { path: 'assistant', name: 'assistant', component: page, meta: { titleKey: 'navigation.assistant', icon: 'pi pi-sparkles', contextPanel: true } },
      { path: 'profile', name: 'profile', component: page, meta: { titleKey: 'navigation.profile', icon: 'pi pi-user' } },
      { path: 'settings', name: 'settings', component: page, meta: { titleKey: 'navigation.settings', icon: 'pi pi-cog' } },
    ],
  },
  {
    path: '/auth',
    component: () => import('@/layouts/AuthLayout.vue'),
    children: [
      { path: 'login', name: 'login', component: page, meta: { titleKey: 'auth.login' } },
      { path: 'forbidden', name: 'forbidden', component: page, meta: { titleKey: 'errors.forbidden' } },
      { path: ':pathMatch(.*)*', name: 'not-found', component: page, meta: { titleKey: 'errors.notFound' } },
    ],
  },
  { path: '/:pathMatch(.*)*', redirect: { name: 'not-found' } },
]

declare module 'vue-router' {
  interface RouteMeta {
    titleKey: string
    icon?: string
    contextPanel?: boolean
    permission?: string
  }
}
