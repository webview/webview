import { mockApi } from './mockApi'

export interface ProjectRecord { id: number; nameKey: string; ownerKey: string; status: 'active' | 'review' | 'planned'; progress: number; due: string }
export interface TaskRecord { id: number; titleKey: string; projectKey: string; priority: 'high' | 'medium' | 'low'; status: 'todo' | 'doing' | 'review' | 'done'; ownerKey: string }
export interface FileRecord { id: number; nameKey: string; type: string; size: string; modifiedKey: string }
export interface TeamRecord { id: number; nameKey: string; initials: string; role: 'owner' | 'admin' | 'member'; teamKey: string; status: 'online' | 'busy' | 'offline'; joined: string }
export interface MessageRecord { id: number; senderKey: string; initials: string; titleKey: string; previewKey: string; timeKey: string; unread: boolean }
export interface AnalyticsData { rows: Array<{ sourceKey: string; visits: string; conversion: string; change: string }>; trend: number[]; channels: Array<{ nameKey: string; value: number }> }

const projects: ProjectRecord[] = [
  { id: 1, nameKey: 'samples.projects.northstar', ownerKey: 'samples.people.lin', status: 'active', progress: 72, due: '2026-08-18' },
  { id: 2, nameKey: 'samples.projects.mobile', ownerKey: 'samples.people.gu', status: 'review', progress: 48, due: '2026-08-28' },
  { id: 3, nameKey: 'samples.projects.brand', ownerKey: 'samples.people.zhou', status: 'planned', progress: 24, due: '2026-09-12' },
  { id: 4, nameKey: 'samples.projects.growth', ownerKey: 'samples.people.shen', status: 'active', progress: 61, due: '2026-09-30' },
]
const tasks: TaskRecord[] = [
  { id: 1, titleKey: 'samples.tasks.prototypeReview', projectKey: 'samples.projects.northstar', priority: 'high', status: 'doing', ownerKey: 'samples.people.lin' },
  { id: 2, titleKey: 'samples.tasks.interactionStates', projectKey: 'samples.projects.designSystem', priority: 'medium', status: 'todo', ownerKey: 'samples.people.zhou' },
  { id: 3, titleKey: 'samples.tasks.growthAnalysis', projectKey: 'samples.projects.growth', priority: 'high', status: 'review', ownerKey: 'samples.people.shen' },
  { id: 4, titleKey: 'samples.tasks.archiveAssets', projectKey: 'samples.projects.brand', priority: 'low', status: 'done', ownerKey: 'samples.people.gu' },
]
const files: FileRecord[] = [
  { id: 1, nameKey: 'samples.files.guidelines', type: 'PDF', size: '8.4 MB', modifiedKey: 'samples.time.today' },
  { id: 2, nameKey: 'samples.files.dashboard', type: 'Figma', size: '24.6 MB', modifiedKey: 'samples.time.yesterday' },
  { id: 3, nameKey: 'samples.files.checklist', type: 'Excel', size: '1.2 MB', modifiedKey: 'samples.time.jul13' },
  { id: 4, nameKey: 'samples.files.illustrations', type: 'Archive', size: '68.3 MB', modifiedKey: 'samples.time.jul11' },
]
const team: TeamRecord[] = [
  { id: 1, nameKey: 'samples.people.lin', initials: 'L', role: 'owner', teamKey: 'samples.teams.design', status: 'online', joined: '2025-03-18' },
  { id: 2, nameKey: 'samples.people.zhou', initials: 'Z', role: 'admin', teamKey: 'samples.teams.design', status: 'online', joined: '2025-06-02' },
  { id: 3, nameKey: 'samples.people.shen', initials: 'S', role: 'member', teamKey: 'samples.teams.growth', status: 'busy', joined: '2025-09-14' },
  { id: 4, nameKey: 'samples.people.gu', initials: 'G', role: 'member', teamKey: 'samples.teams.engineering', status: 'offline', joined: '2026-01-08' },
]
const messages: MessageRecord[] = [
  { id: 1, senderKey: 'samples.people.zhou', initials: 'Z', titleKey: 'samples.messages.reviewTitle', previewKey: 'samples.messages.reviewPreview', timeKey: 'samples.time.now', unread: true },
  { id: 2, senderKey: 'samples.people.shen', initials: 'S', titleKey: 'samples.messages.reportTitle', previewKey: 'samples.messages.reportPreview', timeKey: 'samples.time.yesterday', unread: true },
  { id: 3, senderKey: 'samples.teams.product', initials: 'P', titleKey: 'samples.messages.syncTitle', previewKey: 'samples.messages.syncPreview', timeKey: 'samples.time.monday', unread: false },
]
const analytics: AnalyticsData = {
  rows: [{ sourceKey: 'samples.channels.direct', visits: '12,482', conversion: '8.4%', change: '+12.6%' }, { sourceKey: 'samples.channels.search', visits: '8,310', conversion: '6.8%', change: '+8.2%' }, { sourceKey: 'samples.channels.social', visits: '5,091', conversion: '4.3%', change: '-2.1%' }],
  trend: [120, 168, 151, 219, 238, 286],
  channels: [{ nameKey: 'samples.channels.direct', value: 45 }, { nameKey: 'samples.channels.search', value: 28 }, { nameKey: 'samples.channels.social', value: 17 }, { nameKey: 'samples.channels.other', value: 10 }],
}

export const filterProjects = (items: ProjectRecord[], query: string, status: string) => items.filter((item) => (status === 'all' || item.status === status) && item.nameKey.toLowerCase().includes(query.trim().toLowerCase()))
export const getProjects = () => mockApi(projects)
export const getTasks = () => mockApi(tasks)
export const getFiles = () => mockApi(files)
export const getTeam = () => mockApi(team)
export const getMessages = () => mockApi(messages)
export const getAnalytics = () => mockApi(analytics)
