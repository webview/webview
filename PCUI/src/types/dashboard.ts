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
  title: string
  project: string
  time: string
  completed: boolean
}

export interface DashboardProject {
  id: number
  name: string
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
