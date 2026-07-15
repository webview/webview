import { describe, expect, it } from 'vitest'
import { useAsyncData } from '@/composables/useAsyncData'

describe('useAsyncData', () => {
  it('captures resolved data and errors', async () => {
    const success = useAsyncData(async () => ['ready'])
    await success.load()
    expect(success.data.value).toEqual(['ready'])
    expect(success.loading.value).toBe(false)

    const failure = useAsyncData(async () => { throw new Error('offline') })
    await failure.load()
    expect(failure.error.value).toBe('offline')
  })
})
