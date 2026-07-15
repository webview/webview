import type { NavigationGroup } from '@/types/navigation'

export const navigationGroups: NavigationGroup[] = [
  {
    labelKey: 'navigation.workspace',
    items: [
      { labelKey: 'navigation.dashboard', icon: 'pi pi-objects-column', to: { name: 'dashboard' } },
      { labelKey: 'navigation.projects', icon: 'pi pi-folder', to: { name: 'projects' }, badge: '8' },
      { labelKey: 'navigation.tasks', icon: 'pi pi-check-circle', to: { name: 'tasks' } },
      { labelKey: 'navigation.files', icon: 'pi pi-file', to: { name: 'files' } },
      { labelKey: 'navigation.calendar', icon: 'pi pi-calendar', to: { name: 'calendar' } },
    ],
  },
  {
    labelKey: 'navigation.insights',
    items: [
      { labelKey: 'navigation.analytics', icon: 'pi pi-chart-line', to: { name: 'analytics' } },
      { labelKey: 'navigation.team', icon: 'pi pi-users', to: { name: 'team' } },
      { labelKey: 'navigation.messages', icon: 'pi pi-inbox', to: { name: 'messages' } },
      { labelKey: 'navigation.assistant', icon: 'pi pi-sparkles', to: { name: 'assistant' } },
    ],
  },
  {
    labelKey: 'navigation.account',
    items: [
      { labelKey: 'navigation.profile', icon: 'pi pi-user', to: { name: 'profile' } },
      { labelKey: 'navigation.settings', icon: 'pi pi-cog', to: { name: 'settings' } },
    ],
  },
]
