import { describe, expect, it } from 'vitest'
import { buildCalendarDays, filterProjects, getProjects } from '@/services/workspace'

describe('workspace services', () => {
  it('returns localizable project records asynchronously', async () => {
    const projects = await getProjects()
    expect(projects[0]?.nameKey).toMatch(/^samples\./)
  })

  it('filters projects by query and status', async () => {
    const projects = await getProjects()
    expect(filterProjects(projects, 'mobile', 'all')).toHaveLength(1)
    expect(filterProjects(projects, '', 'review')).toHaveLength(1)
  })

  it('builds a different calendar grid for each month', () => {
    const july = buildCalendarDays(new Date(2026, 6, 1), [])
    const august = buildCalendarDays(new Date(2026, 7, 1), [])
    expect(july[0]?.isoDate).not.toBe(august[0]?.isoDate)
    expect(july).toHaveLength(42)
  })
})
