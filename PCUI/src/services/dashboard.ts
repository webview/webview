import { mockApi } from './mockApi'
import type { DashboardData } from '@/types/dashboard'

const dashboardData: DashboardData = {
  metrics: [
    { id: 'projects', labelKey: 'dashboard.metrics.projects', value: '12', helperKey: 'dashboard.metrics.projectsHelp', icon: 'pi pi-folder', tone: 'primary' },
    { id: 'tasks', labelKey: 'dashboard.metrics.tasks', value: '84', helperKey: 'dashboard.metrics.tasksHelp', icon: 'pi pi-check-circle', tone: 'success' },
    { id: 'focus', labelKey: 'dashboard.metrics.focus', value: '36.5h', helperKey: 'dashboard.metrics.focusHelp', icon: 'pi pi-clock', tone: 'warning' },
    { id: 'members', labelKey: 'dashboard.metrics.members', value: '24', helperKey: 'dashboard.metrics.membersHelp', icon: 'pi pi-users', tone: 'violet' },
  ],
  progress: [32, 41, 48, 44, 63, 72, 86],
  tasks: [
    { id: 1, titleKey: 'samples.tasks.prototypeReview', projectKey: 'samples.projects.northstar', time: '09:30', completed: true },
    { id: 2, titleKey: 'samples.tasks.designSystem', projectKey: 'samples.projects.designSystem', time: '13:00', completed: false },
    { id: 3, titleKey: 'samples.tasks.teamSync', projectKey: 'samples.projects.productTeam', time: '16:30', completed: false },
  ],
  projects: [
    { id: 1, nameKey: 'samples.projects.northstar', team: ['L', 'Z', 'S'], progress: 72, status: 'active' },
    { id: 2, nameKey: 'samples.projects.mobile', team: ['G', 'L'], progress: 48, status: 'review' },
    { id: 3, nameKey: 'samples.projects.brand', team: ['Z', 'Y'], progress: 24, status: 'planned' },
  ],
}

export const getDashboardData = () => mockApi(dashboardData)
