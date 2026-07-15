import { readFileSync } from 'node:fs'
import { resolve } from 'node:path'
import { describe, expect, it } from 'vitest'

const css = readFileSync(resolve('src/styles/main.css'), 'utf8')

describe('typography system', () => {
  it('defines the approved font stack and semantic scale', () => {
    expect(css).toContain('"Segoe UI Variable"')
    expect(css).toContain('"Microsoft YaHei UI"')
    expect(css).toContain('--text-caption: 0.75rem')
    expect(css).toContain('--text-ui: 0.875rem')
    expect(css).toContain('--text-body: 1rem')
    expect(css).toContain('--text-section: 1.25rem')
    expect(css).toContain('--text-page: 1.75rem')
  })
})
