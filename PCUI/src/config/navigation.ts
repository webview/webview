import type { NavigationGroup } from '@/types/navigation'

export const navigationGroups: NavigationGroup[] = [
  {
    labelKey: 'navigation.workspace',
    items: [
      // 工作台：汇总关键指标、今日任务和最近项目。
      { labelKey: 'navigation.dashboard', icon: 'pi pi-objects-column', to: { name: 'dashboard' } },
      // 项目管理：集中跟踪项目进度、成员和交付时间。
      { labelKey: 'navigation.projects', icon: 'pi pi-folder', to: { name: 'projects' }, badge: '8' },
      // 任务中心：使用看板或列表组织团队任务。
      { labelKey: 'navigation.tasks', icon: 'pi pi-check-circle', to: { name: 'tasks' } },
      // 文件管理：统一管理项目文件夹和团队资源。
      { labelKey: 'navigation.files', icon: 'pi pi-file', to: { name: 'files' } },
      // 日历日程：安排会议、评审和项目节点。
      { labelKey: 'navigation.calendar', icon: 'pi pi-calendar', to: { name: 'calendar' } },
    ],
  },
  {
    labelKey: 'navigation.insights',
    items: [
      // 数据分析：查看工作空间趋势与渠道表现。
      { labelKey: 'navigation.analytics', icon: 'pi pi-chart-line', to: { name: 'analytics' } },
      // 团队成员：管理成员、角色和工作空间权限。
      { labelKey: 'navigation.team', icon: 'pi pi-users', to: { name: 'team' } },
      // 消息中心：集中处理团队消息和系统通知。
      { labelKey: 'navigation.messages', icon: 'pi pi-inbox', to: { name: 'messages' } },
      // AI 助手：整理信息、规划任务并辅助日常工作。
      { labelKey: 'navigation.assistant', icon: 'pi pi-sparkles', to: { name: 'assistant' } },
    ],
  },
  {
    labelKey: 'navigation.account',
    items: [
      // 个人中心：维护个人资料和工作偏好。
      { labelKey: 'navigation.profile', icon: 'pi pi-user', to: { name: 'profile' } },
      // 系统设置：配置主题、语言、通知和快捷键。
      { labelKey: 'navigation.settings', icon: 'pi pi-cog', to: { name: 'settings' } },
    ],
  },
]
