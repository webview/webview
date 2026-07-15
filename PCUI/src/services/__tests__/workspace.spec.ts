import { describe, expect, it } from 'vitest'
import { filterProjects, getProjects } from '@/services/workspace'

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
})
