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
    { id: 1, title: '完成产品原型评审', project: 'Northstar 2.0', time: '09:30', completed: true },
    { id: 2, title: '更新设计系统组件', project: '设计规范', time: '13:00', completed: false },
    { id: 3, title: '团队周会同步', project: '产品团队', time: '16:30', completed: false },
  ],
  projects: [
    { id: 1, name: 'Northstar 2.0', team: ['林', '周', '沈'], progress: 72, status: 'active' },
    { id: 2, name: '移动端体验升级', team: ['顾', '林'], progress: 48, status: 'review' },
    { id: 3, name: '品牌资源中心', team: ['周', '叶'], progress: 24, status: 'planned' },
  ],
}

export const getDashboardData = () => mockApi(dashboardData)
