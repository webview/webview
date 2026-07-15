import { describe, expect, it } from 'vitest'
import { routes } from '@/router/routes'

describe('workspace routes', () => {
  it.each([
    ['team', 'TeamView'],
    ['messages', 'MessagesView'],
    ['assistant', 'AssistantView'],
    ['profile', 'ProfileView'],
    ['settings', 'SettingsView'],
  ])('loads %s as its own view', async (name, expectedComponentName) => {
    const route = routes[0]?.children?.find((item) => item.name === name)
    const module = await (route?.component as () => Promise<{ default: { __name?: string } }>)()
    expect(module.default.__name).toBe(expectedComponentName)
  })
})
