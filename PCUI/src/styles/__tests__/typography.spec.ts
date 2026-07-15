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

  it('maps shared component roles to semantic tokens', () => {
    expect(css).toContain('.page-title')
    expect(css).toContain('font-size: var(--text-page)')
    expect(css).toContain('.page-description')
    expect(css).toContain('.metric-value')
    expect(css).toContain('font-variant-numeric: tabular-nums')
  })

  it('uses the UI text role for shell and PrimeVue controls', () => {
    expect(css).toContain('.p-button.p-component,.p-inputtext.p-component')
    expect(css).toContain('.p-datatable .p-datatable-tbody > tr > td')
    expect(css).toMatch(/\.panel-heading h2\s*\{[^}]*font-size: var\(--text-section\)/s)
  })

  it('styles sidebar group labels as readable muted captions', () => {
    expect(css).toContain('--color-muted-color: var(--p-text-muted-color)')
    expect(css).toMatch(/\.sidebar-group-label\s*\{[^}]*font-size: var\(--text-caption\)/s)
    expect(css).toMatch(/\.sidebar-group-label\s*\{[^}]*line-height: var\(--leading-caption\)/s)
    expect(css).toMatch(/\.sidebar-group-label\s*\{[^}]*color: var\(--p-text-muted-color\)/s)
  })
})
