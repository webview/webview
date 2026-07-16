import { describe, expect, it } from 'vitest'
import { existsSync, readFileSync } from 'node:fs'
import { join } from 'node:path'

describe('locale resource boundary', () => {
  it('keeps product translations out of legacy TypeScript modules', () => {
    expect(existsSync(join(process.cwd(), 'src/locales/zh-CN.ts'))).toBe(false)
    expect(existsSync(join(process.cwd(), 'src/locales/en-US.ts'))).toBe(false)
  })

  it('keeps PrimeVue TypeScript limited to JSON wiring', () => {
    const source = readFileSync(join(process.cwd(), 'src/locales/primevue.ts'), 'utf8')
    expect(source).not.toMatch(/\p{Script=Han}/u)
    expect(source).not.toMatch(/accept\s*:\s*['"]/) 
    expect(source).toContain("from './primevue/zh-CN.json'")
    expect(source).toContain("from './primevue/en-US.json'")
  })
})
