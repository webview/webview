import { describe, expect, it } from 'vitest'
import { readdirSync, readFileSync } from 'node:fs'
import { extname, join } from 'node:path'

const collect = (directory: string): string[] => readdirSync(directory, { withFileTypes: true }).flatMap((entry) => {
  const path = join(directory, entry.name)
  if (entry.isDirectory()) {
    if (entry.name === 'locales' || entry.name === '__tests__') return []
    return collect(path)
  }
  return ['.ts', '.vue'].includes(extname(entry.name)) ? [path] : []
})

const removeComments = (source: string) => source
  .replace(/\/\*[\s\S]*?\*\//g, '')
  .replace(/^\s*\/\/.*$/gm, '')

describe('localization boundaries', () => {
  it('keeps Chinese interface text inside locale resources', () => {
    const offenders = collect(join(process.cwd(), 'src')).filter((path) => (
      /\p{Script=Han}/u.test(removeComments(readFileSync(path, 'utf8')))
    ))
    expect(offenders).toEqual([])
  })
})
