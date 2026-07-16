import { windowBridge } from '@/bridge/windowBridge'
import type { useStartupStore } from '@/stores/startup'
import type { StartupStepKey } from '@/stores/startup'

type StartupStore = ReturnType<typeof useStartupStore>

export interface StartupStep {
  key: StartupStepKey
  progress: number
  run: (signal: AbortSignal) => Promise<void> | void
}

export class StartupInitializationError extends Error {
  constructor(
    public readonly code: string,
    public readonly messageKey: string,
  ) {
    super(code)
  }
}

interface StartupOptions {
  store: StartupStore
  steps?: StartupStep[]
  minimumDurationMs?: number
  timeoutMs?: number
}

const delay = (milliseconds: number) => new Promise<void>((resolve) => {
  globalThis.setTimeout(resolve, milliseconds)
})

export const defaultStartupSteps = (): StartupStep[] => [
  { key: 'preferences', progress: 20, run: async () => {} },
  { key: 'profile', progress: 40, run: async () => {} },
  { key: 'workspace', progress: 75, run: async () => {} },
  {
    key: 'host',
    progress: 100,
    run: async () => {
      windowBridge.isHostAvailable()
    },
  },
]

const normalizeError = (error: unknown) => error instanceof StartupInitializationError
  ? error
  : new StartupInitializationError('STARTUP_FAILED', 'startup.errors.generic')

export const runStartupInitialization = async ({
  store,
  steps = defaultStartupSteps(),
  minimumDurationMs = 1200,
  timeoutMs = 8000,
}: StartupOptions): Promise<boolean> => {
  if (!store.begin()) return false

  const controller = new AbortController()
  let timeoutId: ReturnType<typeof setTimeout> | undefined
  const executeSteps = async () => {
    for (const step of steps) {
      store.startStep(step.key)
      await step.run(controller.signal)
      if (controller.signal.aborted) {
        throw new StartupInitializationError('STARTUP_TIMEOUT', 'startup.errors.timeout')
      }
      store.advance(step.key, step.progress)
    }
  }
  const timeout = new Promise<never>((_, reject) => {
    timeoutId = globalThis.setTimeout(() => {
      controller.abort()
      reject(new StartupInitializationError('STARTUP_TIMEOUT', 'startup.errors.timeout'))
    }, timeoutMs)
  })

  try {
    await Promise.race([
      Promise.all([executeSteps(), delay(minimumDurationMs)]),
      timeout,
    ])
    store.succeed()
    return true
  } catch (error) {
    const normalized = normalizeError(error)
    store.fail(normalized.code, normalized.messageKey)
    return false
  } finally {
    if (timeoutId !== undefined) globalThis.clearTimeout(timeoutId)
  }
}
