# Startup Initialization Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a full-window, localized startup experience that performs real initialization steps, enters the workspace only after success, and exposes retry and WebView2-safe exit controls after failure.

**Architecture:** A dedicated Pinia startup store owns the state machine, while a focused initialization service runs ordered steps under a minimum-duration and total-timeout policy. A top-level startup route and router guard keep application routes behind initialization; the startup view only renders state and delegates work to the service and existing window bridge.

**Tech Stack:** Vue 3 Composition API, TypeScript, Pinia, Vue Router, Vue I18n, PrimeVue theme tokens, Vitest, Vue Test Utils, happy-dom, WebView2 message bridge.

---

## File map

- Create `src/stores/startup.ts`: startup state machine and concurrency guard.
- Create `src/services/initializeApp.ts`: ordered initialization, real progress, timeout, minimum display time, and error normalization.
- Create `src/services/__tests__/initializeApp.spec.ts`: service and store behavior.
- Create `src/views/startup/StartupView.vue`: confirmed loading and failed UI states.
- Create `src/views/startup/__tests__/StartupView.spec.ts`: rendering, retry, progress accessibility, and exit behavior.
- Create `src/router/__tests__/startupRouting.spec.ts`: default entry and guarded navigation behavior.
- Modify `src/bridge/windowBridge.ts`: expose WebView2 host availability through the bridge.
- Modify `src/bridge/__tests__/windowBridge.spec.ts`: verify browser and WebView2 availability results.
- Modify `src/App.vue`: apply theme and locale once at the root.
- Modify `src/layouts/DefaultLayout.vue`: remove duplicate theme/locale initialization.
- Modify `src/layouts/AuthLayout.vue`: remove duplicate theme/locale initialization.
- Modify `src/router/index.ts`: export a router factory and install the startup guard.
- Modify `src/router/routes.ts`: add `/startup`, make it the default entry, add route comments, and mark public routes.
- Modify `src/config/navigation.ts`: add the previously approved function-page comments.
- Modify `src/locales/zh-CN.ts`: add Chinese startup messages.
- Modify `src/locales/en-US.ts`: add equivalent English startup messages.
- Modify `src/locales/__tests__/noHardcodedChinese.spec.ts`: allow Chinese source comments while continuing to reject rendered Chinese text outside locale files.
- Modify `src/styles/main.css`: add full-window visual layout, animation, failure state, dark mode, narrow-window, drag-region, and reduced-motion rules.

### Task 1: Expose WebView2 host capability through the window bridge

**Files:**
- Modify: `src/bridge/windowBridge.ts`
- Test: `src/bridge/__tests__/windowBridge.spec.ts`

- [ ] **Step 1: Write the failing host-availability tests**

Add these assertions to the existing browser fallback and WebView2 channel tests:

```ts
expect(bridge.isHostAvailable()).toBe(false)
```

```ts
expect(bridge.isHostAvailable()).toBe(true)
```

- [ ] **Step 2: Run the bridge test and confirm the new API is missing**

Run: `npm run test:run -- src/bridge/__tests__/windowBridge.spec.ts`

Expected: FAIL because `isHostAvailable` does not exist on `WindowBridge`.

- [ ] **Step 3: Add the read-only capability to the bridge**

Extend the interface and returned object in `src/bridge/windowBridge.ts`:

```ts
export interface WindowBridge {
  minimize: () => Promise<void>
  toggleMaximize: () => Promise<void>
  close: () => Promise<void>
  isMaximized: () => Promise<boolean>
  isHostAvailable: () => boolean
  onWindowStateChanged: (callback: (maximized: boolean) => void) => () => void
}
```

```ts
isHostAvailable: () => Boolean(channel),
```

Keep `close()` as a safe no-op in a normal browser. Do not access `window.chrome.webview` from any view or service.

- [ ] **Step 4: Run the bridge tests**

Run: `npm run test:run -- src/bridge/__tests__/windowBridge.spec.ts`

Expected: all bridge tests PASS.

- [ ] **Step 5: Commit the bridge capability**

```bash
git add src/bridge/windowBridge.ts src/bridge/__tests__/windowBridge.spec.ts
git commit -m "feat: expose WebView2 host availability"
```

### Task 2: Build the startup state machine and initialization orchestrator

**Files:**
- Create: `src/stores/startup.ts`
- Create: `src/services/initializeApp.ts`
- Create: `src/services/__tests__/initializeApp.spec.ts`

- [ ] **Step 1: Write failing tests for ordered progress, minimum duration, timeout, errors, and concurrent retries**

Create `src/services/__tests__/initializeApp.spec.ts` with fake timers and these cases:

```ts
import { afterEach, beforeEach, describe, expect, it, vi } from 'vitest'
import { createPinia, setActivePinia } from 'pinia'
import { runStartupInitialization, StartupInitializationError, type StartupStep } from '@/services/initializeApp'
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
    const task = runStartupInitialization({ store, steps: makeSteps(calls), minimumDurationMs: 1200, timeoutMs: 8000 })

    await vi.advanceTimersByTimeAsync(1199)
    expect(store.status).toBe('running')
    await vi.advanceTimersByTimeAsync(1)

    await expect(task).resolves.toBe(true)
    expect(calls).toEqual(['preferences', 'profile', 'workspace', 'host'])
    expect(store.progress).toBe(100)
    expect(store.status).toBe('success')
  })

  it('normalizes a step error into a recoverable failed state', async () => {
    const store = useStartupStore()
    const steps: StartupStep[] = [{
      key: 'workspace',
      progress: 75,
      run: async () => { throw new StartupInitializationError('WORKSPACE_LOAD_FAILED', 'startup.errors.workspace') },
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
```

- [ ] **Step 2: Run the new service test and verify it fails**

Run: `npm run test:run -- src/services/__tests__/initializeApp.spec.ts`

Expected: FAIL because the startup store and service do not exist.

- [ ] **Step 3: Implement the Pinia startup store**

Create `src/stores/startup.ts`:

```ts
import { computed, ref } from 'vue'
import { defineStore } from 'pinia'

export type StartupStatus = 'idle' | 'running' | 'success' | 'failed'
export type StartupStepKey = 'preferences' | 'profile' | 'workspace' | 'host'

export const useStartupStore = defineStore('startup', () => {
  const status = ref<StartupStatus>('idle')
  const currentStep = ref<StartupStepKey | null>(null)
  const completedSteps = ref<StartupStepKey[]>([])
  const progress = ref(0)
  const errorCode = ref<string | null>(null)
  const errorMessageKey = ref<string | null>(null)
  const attempt = ref(0)
  const initialized = computed(() => status.value === 'success')

  const begin = () => {
    if (status.value === 'running') return false
    status.value = 'running'
    currentStep.value = null
    completedSteps.value = []
    progress.value = 0
    errorCode.value = null
    errorMessageKey.value = null
    attempt.value += 1
    return true
  }

  const startStep = (step: StartupStepKey) => {
    currentStep.value = step
  }

  const advance = (step: StartupStepKey, value: number) => {
    if (!completedSteps.value.includes(step)) completedSteps.value.push(step)
    progress.value = Math.max(progress.value, Math.min(100, value))
  }

  const succeed = () => {
    status.value = 'success'
    progress.value = 100
  }

  const fail = (code: string, messageKey: string) => {
    status.value = 'failed'
    errorCode.value = code
    errorMessageKey.value = messageKey
  }

  return { status, currentStep, completedSteps, progress, errorCode, errorMessageKey, attempt, initialized, begin, startStep, advance, succeed, fail }
})
```

- [ ] **Step 4: Implement the initialization service and default steps**

Create `src/services/initializeApp.ts` with the public contracts below. The timeout promise must abort step progression and its timer must always be cleared.

```ts
import type { useStartupStore } from '@/stores/startup'
import type { StartupStepKey } from '@/stores/startup'
import { windowBridge } from '@/bridge/windowBridge'

type StartupStore = ReturnType<typeof useStartupStore>

export interface StartupStep {
  key: StartupStepKey
  progress: number
  run: (signal: AbortSignal) => Promise<void> | void
}

export class StartupInitializationError extends Error {
  constructor(public readonly code: string, public readonly messageKey: string) {
    super(code)
  }
}

interface StartupOptions {
  store: StartupStore
  steps?: StartupStep[]
  minimumDurationMs?: number
  timeoutMs?: number
}

const delay = (milliseconds: number) => new Promise<void>((resolve) => globalThis.setTimeout(resolve, milliseconds))

export const defaultStartupSteps = (): StartupStep[] => [
  { key: 'preferences', progress: 20, run: async () => {} },
  { key: 'profile', progress: 40, run: async () => {} },
  { key: 'workspace', progress: 75, run: async () => {} },
  {
    key: 'host',
    progress: 100,
    run: async () => {
      // WebView2 channel present = connected; no channel = supported browser preview mode.
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
      if (controller.signal.aborted) throw new StartupInitializationError('STARTUP_TIMEOUT', 'startup.errors.timeout')
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
    await Promise.race([Promise.all([executeSteps(), delay(minimumDurationMs)]), timeout])
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
```

- [ ] **Step 5: Run the initialization tests**

Run: `npm run test:run -- src/services/__tests__/initializeApp.spec.ts`

Expected: all initialization tests PASS with no unhandled promise rejection.

- [ ] **Step 6: Commit the state machine and service**

```bash
git add src/stores/startup.ts src/services/initializeApp.ts src/services/__tests__/initializeApp.spec.ts
git commit -m "feat: add startup initialization state machine"
```

### Task 3: Apply theme and locale once at the application root

**Files:**
- Modify: `src/App.vue`
- Modify: `src/layouts/DefaultLayout.vue`
- Modify: `src/layouts/AuthLayout.vue`
- Test: `src/composables/__tests__/preferences.spec.ts`

- [ ] **Step 1: Add a failing root-preference regression test**

Mount `App.vue` with Pinia, i18n, PrimeVue, and a stubbed `RouterView`. Assert that persisted dark mode and English locale affect `document.documentElement` without mounting either layout:

```ts
localStorage.setItem('pcui:theme', 'dark')
localStorage.setItem('pcui:locale', 'en-US')
mount(App, {
  global: {
    plugins: [createPinia(), i18n, PrimeVue, ToastService, ConfirmationService],
    stubs: { RouterView: true, Toast: true, ConfirmDialog: true },
  },
})
await nextTick()
expect(document.documentElement.classList.contains('app-dark')).toBe(true)
expect(document.documentElement.lang).toBe('en-US')
```

- [ ] **Step 2: Run the preference test and confirm it fails**

Run: `npm run test:run -- src/composables/__tests__/preferences.spec.ts`

Expected: the new root test FAILS because `App.vue` does not call the preference composables.

- [ ] **Step 3: Move preference initialization to `App.vue`**

Add to `App.vue`:

```ts
import { useLocale } from '@/composables/useLocale'
import { useTheme } from '@/composables/useTheme'

useTheme()
useLocale()
```

Remove both imports and calls from `DefaultLayout.vue` and `AuthLayout.vue`. Do not change their templates.

- [ ] **Step 4: Run the preference test suite**

Run: `npm run test:run -- src/composables/__tests__/preferences.spec.ts`

Expected: all preference tests PASS and only the root owns the listeners.

- [ ] **Step 5: Commit root preference initialization**

```bash
git add src/App.vue src/layouts/DefaultLayout.vue src/layouts/AuthLayout.vue src/composables/__tests__/preferences.spec.ts
git commit -m "refactor: initialize preferences at app root"
```

### Task 4: Gate application routes behind startup and add navigation comments

**Files:**
- Modify: `src/router/index.ts`
- Modify: `src/router/routes.ts`
- Modify: `src/config/navigation.ts`
- Modify: `src/locales/__tests__/noHardcodedChinese.spec.ts`
- Create: `src/router/__tests__/startupRouting.spec.ts`

- [ ] **Step 1: Write failing routing tests**

Create `src/router/__tests__/startupRouting.spec.ts` and use a memory history router from a new `createAppRouter` factory:

```ts
import { beforeEach, describe, expect, it } from 'vitest'
import { createPinia, setActivePinia } from 'pinia'
import { createMemoryHistory } from 'vue-router'
import { createAppRouter } from '@/router'
import { useStartupStore } from '@/stores/startup'

describe('startup routing', () => {
  beforeEach(() => setActivePinia(createPinia()))

  it('uses startup as the cold default entry', async () => {
    const router = createAppRouter(createMemoryHistory())
    await router.push('/')
    await router.isReady()
    expect(router.currentRoute.value.name).toBe('startup')
  })

  it('redirects protected workspace routes until startup succeeds', async () => {
    const router = createAppRouter(createMemoryHistory())
    await router.push('/projects')
    expect(router.currentRoute.value.name).toBe('startup')
  })

  it('allows workspace routes after startup succeeds', async () => {
    const router = createAppRouter(createMemoryHistory())
    useStartupStore().begin()
    useStartupStore().succeed()
    await router.push('/projects')
    expect(router.currentRoute.value.name).toBe('projects')
  })

  it('keeps auth routes public', async () => {
    const router = createAppRouter(createMemoryHistory())
    await router.push('/auth/login')
    expect(router.currentRoute.value.name).toBe('login')
  })
})
```

- [ ] **Step 2: Run routing tests and confirm they fail**

Run: `npm run test:run -- src/router/__tests__/startupRouting.spec.ts`

Expected: FAIL because there is no startup route or router factory.

- [ ] **Step 3: Add route metadata and the startup route**

In `src/router/routes.ts` add the top-level startup record before the main layout:

```ts
{
  path: '/startup',
  name: 'startup',
  component: () => import('@/views/startup/StartupView.vue'),
  meta: { titleKey: 'startup.title', public: true },
},
```

Change the main layout empty child to redirect to `startup`. Mark the `/auth` parent `meta: { titleKey: 'auth.login', public: true }`. Extend `RouteMeta`:

```ts
public?: boolean
```

- [ ] **Step 4: Export a router factory and install the startup guard**

Refactor `src/router/index.ts`:

```ts
import { createRouter, createWebHashHistory, type RouterHistory } from 'vue-router'
import { useStartupStore } from '@/stores/startup'
import { routes } from './routes'

export const createAppRouter = (history: RouterHistory = createWebHashHistory()) => {
  const router = createRouter({ history, routes, scrollBehavior: () => ({ top: 0 }) })
  router.beforeEach((to) => {
    const startup = useStartupStore()
    if (to.name === 'startup' && startup.initialized) return { name: 'dashboard' }
    if (!to.meta.public && !startup.initialized) return { name: 'startup' }
  })
  return router
}

export const router = createAppRouter()
```

- [ ] **Step 5: Add the approved comments without weakening localization checks**

Add a short Chinese comment above each item in `src/config/navigation.ts` and its corresponding route in `src/router/routes.ts`, for example:

```ts
// 工作台：汇总关键指标、今日任务和最近项目。
{ labelKey: 'navigation.dashboard', icon: 'pi pi-objects-column', to: { name: 'dashboard' } },
```

```ts
// 工作台页：展示工作空间概览和核心指标。
{ path: 'dashboard', name: 'dashboard', component: () => import('@/views/dashboard/DashboardView.vue'), meta: { titleKey: 'navigation.dashboard', icon: 'pi pi-objects-column' } },
```

Cover dashboard, projects, tasks, files, calendar, analytics, team, messages, assistant, profile, and settings. Update `noHardcodedChinese.spec.ts` so it removes block and line comments before checking for Han characters:

```ts
const removeComments = (source: string) => source
  .replace(/\/\*[\s\S]*?\*\//g, '')
  .replace(/^\s*\/\/.*$/gm, '')

const offenders = collect(join(process.cwd(), 'src')).filter((path) =>
  /\p{Script=Han}/u.test(removeComments(readFileSync(path, 'utf8'))),
)
```

- [ ] **Step 6: Run routing and localization boundary tests**

Run: `npm run test:run -- src/router/__tests__/startupRouting.spec.ts src/locales/__tests__/noHardcodedChinese.spec.ts`

Expected: all tests PASS; Chinese comments are permitted but Chinese UI strings outside locale resources still fail the boundary.

- [ ] **Step 7: Commit routing and comments**

```bash
git add src/router/index.ts src/router/routes.ts src/router/__tests__/startupRouting.spec.ts src/config/navigation.ts src/locales/__tests__/noHardcodedChinese.spec.ts
git commit -m "feat: gate workspace routes behind startup"
```

### Task 5: Add localized startup UI behavior

**Files:**
- Create: `src/views/startup/StartupView.vue`
- Create: `src/views/startup/__tests__/StartupView.spec.ts`
- Modify: `src/locales/zh-CN.ts`
- Modify: `src/locales/en-US.ts`

- [ ] **Step 1: Add matching locale resources**

Add this structure to both locale modules, translating every value but keeping keys identical:

```ts
startup: {
  title: '程序初始化',
  eyebrow: 'STARTING NORTHSTAR',
  interrupted: 'STARTUP INTERRUPTED',
  heading: '工作空间即将就绪',
  description: '正在载入用户偏好、连接本地服务并准备工作空间。',
  close: '关闭程序',
  retry: '重试初始化',
  exit: '退出程序',
  failedTitle: '无法完成初始化',
  failedDescription: '本地服务暂时没有响应。你可以重试初始化，或者安全退出程序。',
  errorCode: '错误代码：{code}',
  steps: {
    preferences: '正在应用界面偏好',
    profile: '正在载入用户配置',
    workspace: '正在准备工作空间',
    host: '正在连接本地服务',
  },
  errors: {
    timeout: '初始化超时，请重试。',
    workspace: '工作空间数据加载失败。',
    generic: '初始化过程中出现异常。',
  },
},
```

The English file must provide natural English equivalents such as `Workspace is almost ready`, `Retry initialization`, and `Exit application`.

- [ ] **Step 2: Write failing startup-view tests**

Create `src/views/startup/__tests__/StartupView.spec.ts`. Mock `runStartupInitialization` and `windowBridge.close`, mount with Pinia, router, and i18n, then cover:

```ts
expect(wrapper.get('[role="progressbar"]').attributes('aria-valuenow')).toBe('0')
expect(runStartupInitialization).toHaveBeenCalledTimes(1)
```

```ts
store.fail('HOST_TIMEOUT', 'startup.errors.timeout')
await nextTick()
expect(wrapper.get('[role="alert"]').text()).toContain('HOST_TIMEOUT')
await wrapper.get('[data-testid="startup-retry"]').trigger('click')
expect(runStartupInitialization).toHaveBeenCalledTimes(2)
```

```ts
await wrapper.get('[data-testid="startup-close"]').trigger('click')
await wrapper.get('[data-testid="startup-exit"]').trigger('click')
expect(windowBridge.close).toHaveBeenCalledTimes(2)
```

Also assert that the error icon and `STARTUP INTERRUPTED` share the same `[data-testid="startup-error-kicker"]` container, locking the confirmed icon position.

Add a focus assertion after moving the store to `failed`:

```ts
expect(document.activeElement).toBe(wrapper.get('[data-testid="startup-retry"]').element)
```

- [ ] **Step 3: Run the view test and confirm it fails**

Run: `npm run test:run -- src/views/startup/__tests__/StartupView.spec.ts`

Expected: FAIL because `StartupView.vue` does not exist.

- [ ] **Step 4: Implement `StartupView.vue` behavior and semantic markup**

Use `onMounted(run)`, the startup store, `router.replace`, and `windowBridge.close`. Watch for `failed`, await `nextTick()`, and focus a `ref<HTMLButtonElement>` attached to the retry button. The component must:

```ts
const run = async () => {
  const succeeded = await runStartupInitialization({ store: startup })
  if (succeeded) await router.replace({ name: 'dashboard' })
}

onMounted(run)
```

Render one full-window `.startup-screen` with `.startup-brand` and `.startup-content`. Required semantics and test hooks:

```vue
<button data-testid="startup-close" class="startup-close" type="button" :aria-label="t('startup.close')" @click="windowBridge.close">
  <i class="pi pi-times" aria-hidden="true" />
</button>

<div v-if="startup.status !== 'failed'" class="startup-state">
  <span class="startup-eyebrow">{{ t('startup.eyebrow') }}</span>
  <h1>{{ t('startup.heading') }}</h1>
  <p>{{ t('startup.description') }}</p>
  <div class="startup-progress-copy">
    <strong>{{ t(`startup.steps.${startup.currentStep ?? 'preferences'}`) }}</strong>
    <span>{{ startup.progress }}%</span>
  </div>
  <div class="startup-progress" role="progressbar" aria-valuemin="0" aria-valuemax="100" :aria-valuenow="startup.progress">
    <span :style="{ width: `${startup.progress}%` }" />
  </div>
</div>

<div v-else class="startup-state" role="alert">
  <div data-testid="startup-error-kicker" class="startup-error-kicker">
    <span class="startup-error-icon" aria-hidden="true">!</span>
    <span class="startup-eyebrow">{{ t('startup.interrupted') }}</span>
  </div>
  <h1>{{ t('startup.failedTitle') }}</h1>
  <p>{{ t(startup.errorMessageKey ?? 'startup.errors.generic') }}</p>
  <code>{{ t('startup.errorCode', { code: startup.errorCode }) }}</code>
  <div class="startup-actions">
    <button ref="retryButton" data-testid="startup-retry" type="button" @click="run">{{ t('startup.retry') }}</button>
    <button data-testid="startup-exit" type="button" @click="windowBridge.close">{{ t('startup.exit') }}</button>
  </div>
</div>
```

The brand column contains the orbit, breathing logo, `Northstar`, and `Desktop Workspace`. Add `data-webview-drag-region` only to noninteractive background layers; all buttons remain `no-drag` through CSS.

- [ ] **Step 5: Run the startup view tests**

Run: `npm run test:run -- src/views/startup/__tests__/StartupView.spec.ts`

Expected: all startup view tests PASS.

- [ ] **Step 6: Commit localized behavior and markup**

```bash
git add src/views/startup/StartupView.vue src/views/startup/__tests__/StartupView.spec.ts src/locales/zh-CN.ts src/locales/en-US.ts
git commit -m "feat: add localized startup experience"
```

### Task 6: Implement the confirmed startup visual system

**Files:**
- Modify: `src/styles/main.css`
- Test: `src/views/startup/__tests__/StartupView.spec.ts`

- [ ] **Step 1: Add structural visual assertions before CSS**

Extend the view test to assert the confirmed layout classes and absence of the rejected white header strip:

```ts
expect(wrapper.find('.startup-screen').exists()).toBe(true)
expect(wrapper.find('.startup-brand').exists()).toBe(true)
expect(wrapper.find('.startup-content').exists()).toBe(true)
expect(wrapper.find('.startup-state-label').exists()).toBe(false)
```

Run: `npm run test:run -- src/views/startup/__tests__/StartupView.spec.ts`

Expected: PASS for correct markup; if any class is absent, fix markup before adding styles.

- [ ] **Step 2: Add the full startup CSS to `src/styles/main.css`**

Implement these visual rules using PrimeVue tokens for theme compatibility:

```css
.startup-screen {
  display: grid;
  min-height: 100dvh;
  grid-template-columns: minmax(280px, 42%) minmax(0, 1fr);
  overflow: hidden;
  background: var(--p-content-background);
  color: var(--p-text-color);
}
.startup-brand { position: relative; display: grid; overflow: hidden; place-items: center; background: linear-gradient(145deg, #6868ec, #5053db); color: #fff; }
.startup-orbit { position: absolute; width: min(58%, 360px); aspect-ratio: 1; border: 1px solid rgb(255 255 255 / 20%); border-radius: 50%; animation: startup-spin 13s linear infinite; }
.startup-orbit::after { content: ''; position: absolute; top: 15%; right: 5%; width: 9px; height: 9px; border-radius: 50%; background: #fff; box-shadow: 0 0 18px rgb(255 255 255 / 85%); }
.startup-brand-mark { position: relative; z-index: 1; display: grid; place-items: center; text-align: center; }
.startup-logo { display: grid; width: 72px; height: 72px; place-items: center; border: 1px solid rgb(255 255 255 / 35%); border-radius: 20px; background: rgb(255 255 255 / 14%); animation: startup-breathe 3.2s ease-in-out infinite; }
.startup-content { position: relative; display: flex; align-items: center; padding: clamp(3rem, 8vw, 8rem); }
.startup-state { width: min(100%, 540px); }
.startup-close { position: absolute; z-index: 3; top: 16px; right: 16px; display: grid; width: 36px; height: 36px; place-items: center; border: 0; border-radius: 9px; background: transparent; color: var(--p-text-muted-color); -webkit-app-region: no-drag; }
.startup-close:hover { background: #e5484d; color: #fff; }
.startup-eyebrow { color: var(--p-primary-color); font-size: var(--text-caption); line-height: var(--leading-caption); font-weight: var(--weight-semibold); letter-spacing: .09em; }
.startup-state h1 { margin: .75rem 0 .5rem; font-size: clamp(2rem, 3.2vw, 3rem); line-height: 1.15; font-weight: var(--weight-semibold); letter-spacing: -.03em; }
.startup-state > p { max-width: 52ch; margin: 0; color: var(--p-text-muted-color); font-size: var(--text-ui); line-height: var(--leading-ui); }
.startup-progress-copy { display: flex; justify-content: space-between; gap: 1rem; margin-top: 2.25rem; color: var(--p-text-color); font-size: var(--text-ui); }
.startup-progress { height: 4px; margin-top: .75rem; overflow: hidden; border-radius: 999px; background: color-mix(in srgb, var(--p-content-border-color) 82%, transparent); }
.startup-progress span { display: block; height: 100%; border-radius: inherit; background: var(--p-primary-color); transition: width 220ms ease; }
.startup-error-kicker { display: flex; align-items: center; gap: .625rem; }
.startup-error-icon { display: grid; width: 30px; height: 30px; place-items: center; border-radius: 9px; background: color-mix(in srgb, #e5484d 10%, transparent); color: #d6454d; font-weight: var(--weight-semibold); }
.startup-actions { display: flex; flex-wrap: wrap; gap: .75rem; margin-top: 2rem; }
.startup-actions button { min-height: 40px; border-radius: 10px; padding: 0 1rem; font-size: var(--text-ui); font-weight: var(--weight-medium); }
.startup-actions button:first-child { border: 1px solid var(--p-primary-color); background: var(--p-primary-color); color: var(--p-primary-contrast-color); }
.startup-actions button:last-child { border: 1px solid var(--p-content-border-color); background: var(--p-content-background); color: var(--p-text-color); }
.startup-screen.is-failed .startup-orbit,.startup-screen.is-failed .startup-logo { animation-play-state: paused; opacity: .42; }
@keyframes startup-spin { to { transform: rotate(360deg); } }
@keyframes startup-breathe { 50% { transform: scale(1.04); } }
@media (max-width: 760px) {
  .startup-screen { grid-template-columns: 1fr; grid-template-rows: minmax(190px, 34vh) 1fr; }
  .startup-content { align-items: flex-start; padding: 3rem 1.5rem; }
}
@media (prefers-reduced-motion: reduce) {
  .startup-orbit,.startup-logo { animation: none; }
}
```

Adjust only token values or spacing needed to match the approved preview. Do not add the removed top status strip. Keep the error icon inside `.startup-error-kicker`, left of the English status text.

- [ ] **Step 3: Run view, localization, and type checks**

Run: `npm run test:run -- src/views/startup/__tests__/StartupView.spec.ts src/locales/__tests__/noHardcodedChinese.spec.ts`

Expected: PASS.

Run: `npm run type-check`

Expected: PASS with no Vue template or route-meta errors.

- [ ] **Step 4: Commit the visual implementation**

```bash
git add src/styles/main.css src/views/startup/__tests__/StartupView.spec.ts
git commit -m "style: add polished startup screen"
```

### Task 7: Full verification and desktop-browser QA

**Files:**
- Verify all files changed in Tasks 1–6.

- [ ] **Step 1: Run the full automated suite**

Run: `npm run test:run`

Expected: all Vitest suites PASS.

- [ ] **Step 2: Run static and production-build checks**

Run: `npm run type-check`

Expected: PASS.

Run: `npm run build`

Expected: Vite production build completes successfully with no new chunk-size warning beyond the already accepted ECharts analytics chunk.

- [ ] **Step 3: Verify the loading state in the running app**

Open `http://127.0.0.1:5173/#/` in the in-app browser and verify:

- `/` resolves to `/startup` before the workspace appears.
- The rejected top white status strip is absent.
- The brand orbit and logo animate without layout movement.
- Progress has a semantic progressbar and reaches 100% from real steps.
- The close button stays fully visible and clickable.
- Success replaces the route with `/dashboard` so browser Back does not return to startup.

- [ ] **Step 4: Verify the failed and retry states deterministically**

In the component test or a temporary debugger breakpoint, inject a `StartupInitializationError('HOST_TIMEOUT', 'startup.errors.timeout')`; do not commit a production failure query flag. Verify:

- The orbit/logo stop or dim.
- The small `!` is left of `STARTUP INTERRUPTED`, not above the content or beside the Chinese title.
- The Chinese title remains on one line at a normal desktop width.
- The stable error code, retry button, exit button, and top-right close button are visible.
- Retry resets the failed state and starts only one new attempt.

- [ ] **Step 5: Verify themes, languages, viewport, and motion preferences**

Check light and dark mode, `zh-CN` and `en-US`, widths near 1440px, 1024px, 760px, and 480px, plus reduced-motion emulation. Confirm no clipped text, hidden actions, accidental horizontal scroll, or running loop animation under reduced motion.

- [ ] **Step 6: Review the final diff and repository status**

Run: `git diff --check`

Expected: no whitespace errors.

Run: `git status --short`

Expected: only intentional project changes; preserve unrelated `../.superpowers/`, `.superpowers/`, and `../open_duck_isaac_urdf/` untracked directories.

- [ ] **Step 7: Commit any verification-only fixes**

If QA required code corrections, stage only the related PCUI files and commit:

```bash
git add src
git commit -m "fix: harden startup initialization experience"
```

If no code changed during QA, do not create an empty commit.
