import { describe, expect, it } from 'vitest'
import { routes } from '@/router/routes'

describe('authentication and error routes', () => {
  it.each([
    ['login', 'LoginView'],
    ['forbidden', 'ForbiddenView'],
    ['not-found', 'NotFoundView'],
  ])('loads %s as its own view', async (name, expectedComponentName) => {
    const route = routes[1]?.children?.find((item) => item.name === name)
    const module = await (route?.component as () => Promise<{ default: { __name?: string } }>)()
    expect(module.default.__name).toBe(expectedComponentName)
  })
})
