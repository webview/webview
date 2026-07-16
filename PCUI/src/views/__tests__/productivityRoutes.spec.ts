import { describe, expect, it } from 'vitest'
import { routes } from '@/router/routes'

const workspaceRoutes = routes.find((route) => route.path === '/')?.children

describe('productivity routes', () => {
  it.each([
    ['dashboard', 'DashboardView'],
    ['projects', 'ProjectsView'],
    ['tasks', 'TasksView'],
    ['files', 'FilesView'],
    ['analytics', 'AnalyticsView'],
    ['calendar', 'CalendarView'],
  ])('loads %s as its own view', async (name, expectedComponentName) => {
    const route = workspaceRoutes?.find((item) => item.name === name)
    const module = await (route?.component as () => Promise<{ default: { __name?: string } }>)()

    expect(module.default.__name).toBe(expectedComponentName)
  })
})
