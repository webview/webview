import { afterEach, beforeEach, describe, expect, it, vi } from 'vitest'
import { createPinia, setActivePinia } from 'pinia'
import {
  runStartupInitialization,
  StartupInitializationError,
  type StartupStep,
} from '@/services/initializeApp'
import { useStartupStore } from '@/stores/startup'

const makeSteps = (calls: string[]): StartupStep[] => [
  { key: 'preferences', progress: 20, run: async () => { calls.push('preferences') } },
  { key: 'profile', progress: 40, run: async () => { calls.push('profile') } },
  { key: 'workspace', progress: 75, run: async () => { calls.push('workspace') } },
  { key: 'host', progress: 100, run: async () => { calls.push('host') } },
]

describe('startup initialization', () => {
  beforeEach(() => {
    setActivePinia(createPinia())
    vi.useFakeTimers()
  })

  afterEach(() => vi.useRealTimers())

  it('runs real steps in order and waits for the minimum display duration', async () => {
    const calls: string[] = []
    const store = useStartupStore()
    const task = runStartupInitialization({
      store,
      steps: makeSteps(calls),
      minimumDurationMs: 1200,
      timeoutMs: 8000,
    })

    await vi.advanceTimersByTimeAsync(1199)
    expect(store.status).toBe('running')
    await vi.advanceTimersByTimeAsync(1)

    await expect(task).resolves.toBe(true)
    expect(calls).toEqual(['preferences', 'profile', 'workspace', 'host'])
    expect(store.progress).toBe(100)
    expect(store.status).toBe('success')
  })

  it('publishes the current step before a slow operation completes', async () => {
    const store = useStartupStore()
    const steps: StartupStep[] = [{
      key: 'workspace',
      progress: 75,
      run: () => new Promise((resolve) => globalThis.setTimeout(resolve, 100)),
    }]
    const task = runStartupInitialization({ store, steps, minimumDurationMs: 0, timeoutMs: 8000 })

    expect(store.currentStep).toBe('workspace')
    expect(store.progress).toBe(0)
    await vi.advanceTimersByTimeAsync(100)
    await expect(task).resolves.toBe(true)
    expect(store.progress).toBe(100)
  })

  it('normalizes a step error into a recoverable failed state', async () => {
    const store = useStartupStore()
    const steps: StartupStep[] = [{
      key: 'workspace',
      progress: 75,
      run: async () => {
        throw new StartupInitializationError('WORKSPACE_LOAD_FAILED', 'startup.errors.workspace')
      },
    }]

    await expect(runStartupInitialization({ store, steps, minimumDurationMs: 0, timeoutMs: 8000 })).resolves.toBe(false)
    expect(store.status).toBe('failed')
    expect(store.errorCode).toBe('WORKSPACE_LOAD_FAILED')
    expect(store.errorMessageKey).toBe('startup.errors.workspace')
  })

  it('fails with STARTUP_TIMEOUT after the total timeout', async () => {
    const store = useStartupStore()
    const steps: StartupStep[] = [{ key: 'host', progress: 100, run: () => new Promise(() => {}) }]
    const task = runStartupInitialization({ store, steps, minimumDurationMs: 0, timeoutMs: 8000 })

    await vi.advanceTimersByTimeAsync(8000)
    await expect(task).resolves.toBe(false)
    expect(store.status).toBe('failed')
    expect(store.errorCode).toBe('STARTUP_TIMEOUT')
    expect(store.errorMessageKey).toBe('startup.errors.timeout')
  })

  it('ignores a second run while initialization is active', async () => {
    const store = useStartupStore()
    const steps: StartupStep[] = [{ key: 'host', progress: 100, run: async () => {} }]
    const first = runStartupInitialization({ store, steps, minimumDurationMs: 100, timeoutMs: 8000 })
    const second = runStartupInitialization({ store, steps, minimumDurationMs: 100, timeoutMs: 8000 })

    await expect(second).resolves.toBe(false)
    expect(store.attempt).toBe(1)
    await vi.advanceTimersByTimeAsync(100)
    await expect(first).resolves.toBe(true)
  })
})
