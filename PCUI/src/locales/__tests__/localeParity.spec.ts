import { describe, expect, it } from 'vitest'
import productZhCN from '@/locales/messages/zh-CN.json'
import productEnUS from '@/locales/messages/en-US.json'
import primeVueZhCN from '@/locales/primevue/zh-CN.json'
import primeVueEnUS from '@/locales/primevue/en-US.json'

const leafKeys = (value: unknown, prefix = ''): string[] => {
  if (Array.isArray(value)) return [prefix]
  if (typeof value !== 'object' || value === null) return [prefix]

  return Object.entries(value).flatMap(([key, child]) => (
    leafKeys(child, prefix ? `${prefix}.${key}` : key)
  ))
}

describe.each([
  ['product', productZhCN, productEnUS],
  ['PrimeVue', primeVueZhCN, primeVueEnUS],
])('%s locale resources', (_name, zhCN, enUS) => {
  it('keeps Chinese and English leaf keys identical', () => {
    expect(leafKeys(zhCN).sort()).toEqual(leafKeys(enUS).sort())
  })
})
