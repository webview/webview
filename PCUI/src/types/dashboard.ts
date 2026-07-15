export interface DashboardMetric {
  id: string
  labelKey: string
  value: string
  helperKey: string
  icon: string
  tone: 'primary' | 'success' | 'warning' | 'violet'
}

export interface DashboardTask {
  id: number
  titleKey: string
  projectKey: string
  time: string
  completed: boolean
}

export interface DashboardProject {
  id: number
  nameKey: string
  team: string[]
  progress: number
  status: 'active' | 'review' | 'planned'
}

export interface DashboardData {
  metrics: DashboardMetric[]
  progress: number[]
  tasks: DashboardTask[]
  projects: DashboardProject[]
}
