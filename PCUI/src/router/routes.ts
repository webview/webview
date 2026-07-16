import type { RouteRecordRaw } from 'vue-router'

export const routes: RouteRecordRaw[] = [
  {
    path: '/startup',
    name: 'startup',
    component: () => import('@/views/startup/StartupView.vue'),
    meta: { titleKey: 'startup.title', public: true },
  },
  {
    path: '/',
    component: () => import('@/layouts/DefaultLayout.vue'),
    children: [
      { path: '', redirect: { name: 'startup' } },
      // 工作台页：展示工作空间概览和核心指标。
      { path: 'dashboard', name: 'dashboard', component: () => import('@/views/dashboard/DashboardView.vue'), meta: { titleKey: 'navigation.dashboard', icon: 'pi pi-objects-column' } },
      // 项目管理页：跟踪项目进度、成员和交付时间。
      { path: 'projects', name: 'projects', component: () => import('@/views/projects/ProjectsView.vue'), meta: { titleKey: 'navigation.projects', icon: 'pi pi-folder' } },
      // 任务中心页：使用看板或列表组织团队任务。
      { path: 'tasks', name: 'tasks', component: () => import('@/views/tasks/TasksView.vue'), meta: { titleKey: 'navigation.tasks', icon: 'pi pi-check-circle' } },
      // 文件管理页：管理项目文件夹和团队资源。
      { path: 'files', name: 'files', component: () => import('@/views/files/FilesView.vue'), meta: { titleKey: 'navigation.files', icon: 'pi pi-file' } },
      // 数据分析页：呈现工作空间趋势和渠道表现。
      { path: 'analytics', name: 'analytics', component: () => import('@/views/analytics/AnalyticsView.vue'), meta: { titleKey: 'navigation.analytics', icon: 'pi pi-chart-line' } },
      // 团队成员页：管理成员、角色和访问权限。
      { path: 'team', name: 'team', component: () => import('@/views/team/TeamView.vue'), meta: { titleKey: 'navigation.team', icon: 'pi pi-users' } },
      // 消息中心页：集中处理消息和系统通知。
      { path: 'messages', name: 'messages', component: () => import('@/views/messages/MessagesView.vue'), meta: { titleKey: 'navigation.messages', icon: 'pi pi-inbox' } },
      // 日历日程页：安排会议、评审和项目节点。
      { path: 'calendar', name: 'calendar', component: () => import('@/views/calendar/CalendarView.vue'), meta: { titleKey: 'navigation.calendar', icon: 'pi pi-calendar' } },
      // AI 助手页：提供信息整理和任务规划能力。
      { path: 'assistant', name: 'assistant', component: () => import('@/views/assistant/AssistantView.vue'), meta: { titleKey: 'navigation.assistant', icon: 'pi pi-sparkles', contextPanel: true } },
      // 个人中心页：维护个人资料和工作偏好。
      { path: 'profile', name: 'profile', component: () => import('@/views/profile/ProfileView.vue'), meta: { titleKey: 'navigation.profile', icon: 'pi pi-user' } },
      // 系统设置页：配置主题、语言、通知和快捷键。
      { path: 'settings', name: 'settings', component: () => import('@/views/settings/SettingsView.vue'), meta: { titleKey: 'navigation.settings', icon: 'pi pi-cog' } },
    ],
  },
  {
    path: '/auth',
    component: () => import('@/layouts/AuthLayout.vue'),
    meta: { titleKey: 'auth.login', public: true },
    children: [
      { path: 'login', name: 'login', component: () => import('@/views/auth/LoginView.vue'), meta: { titleKey: 'auth.login' } },
      { path: 'forbidden', name: 'forbidden', component: () => import('@/views/errors/ForbiddenView.vue'), meta: { titleKey: 'errors.forbidden' } },
      { path: ':pathMatch(.*)*', name: 'not-found', component: () => import('@/views/errors/NotFoundView.vue'), meta: { titleKey: 'errors.notFound' } },
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
    public?: boolean
  }
}
