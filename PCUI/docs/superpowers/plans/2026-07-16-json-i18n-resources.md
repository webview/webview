# JSON I18n Resources Migration Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Move every Chinese and English UI translation out of TypeScript objects into JSON resources while preserving Vue I18n, PrimeVue locale synchronization, and all existing translation keys.

**Architecture:** Product messages and PrimeVue messages each use one JSON file per locale. TypeScript locale modules become wiring-only files that import JSON and expose the existing runtime objects; parity and boundary tests prevent missing keys or translation literals from returning to TypeScript.

**Tech Stack:** Vue 3, Vue I18n 11, PrimeVue 4, TypeScript 5.9 JSON modules, Vite 8, Vitest.

---

## File map

- Create `src/locales/messages/zh-CN.json`: all Chinese product UI messages.
- Create `src/locales/messages/en-US.json`: all English product UI messages.
- Create `src/locales/primevue/zh-CN.json`: Chinese PrimeVue component locale.
- Create `src/locales/primevue/en-US.json`: complete English PrimeVue component locale.
- Create `src/locales/__tests__/localeParity.spec.ts`: recursive product and PrimeVue key parity.
- Create `src/locales/__tests__/localeResourceBoundary.spec.ts`: legacy TS files absent and locale wiring free of translation literals.
- Modify `src/locales/index.ts`: load product JSON files.
- Modify `src/locales/primevue.ts`: load PrimeVue JSON files and export the existing locale map.
- Delete `src/locales/zh-CN.ts`: replaced by JSON.
- Delete `src/locales/en-US.ts`: replaced by JSON.

### Task 1: Lock product language parity with a failing test

**Files:**
- Create: `src/locales/__tests__/localeParity.spec.ts`

- [ ] **Step 1: Write the parity test before creating JSON resources**

Create `src/locales/__tests__/localeParity.spec.ts`:

```ts
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
```

- [ ] **Step 2: Run the test and verify the missing resources are detected**

Run: `npm run test:run -- src/locales/__tests__/localeParity.spec.ts`

Expected: FAIL because `messages/*.json` and `primevue/*.json` do not exist.

### Task 2: Migrate product translations to JSON

**Files:**
- Create: `src/locales/messages/zh-CN.json`
- Create: `src/locales/messages/en-US.json`
- Modify: `src/locales/index.ts`

- [ ] **Step 1: Mechanically serialize the existing product message objects**

Use the installed Node runtime's TypeScript stripping to perform a lossless bulk conversion:

```powershell
node --experimental-strip-types --input-type=module -e "import { mkdirSync, writeFileSync } from 'node:fs'; mkdirSync('src/locales/messages', { recursive: true }); const zh = (await import('./src/locales/zh-CN.ts')).default; const en = (await import('./src/locales/en-US.ts')).default; writeFileSync('src/locales/messages/zh-CN.json', JSON.stringify(zh, null, 2) + '\n', 'utf8'); writeFileSync('src/locales/messages/en-US.json', JSON.stringify(en, null, 2) + '\n', 'utf8');"
```

This is a mechanical format conversion only. Do not rename keys or rewrite translations during this step.

- [ ] **Step 2: Point Vue I18n at product JSON files**

Change only the imports in `src/locales/index.ts`:

```ts
import { createI18n } from 'vue-i18n'
import enUS from './messages/en-US.json'
import zhCN from './messages/zh-CN.json'

export const i18n = createI18n({
  legacy: false,
  locale: 'zh-CN',
  fallbackLocale: 'en-US',
  messages: { 'zh-CN': zhCN, 'en-US': enUS },
})
```

- [ ] **Step 3: Run existing localization and preference tests**

Run: `npm run test:run -- src/locales/__tests__/noHardcodedChinese.spec.ts src/composables/__tests__/preferences.spec.ts`

Expected: PASS; Vue I18n and `document.documentElement.lang` behavior remain unchanged.

### Task 3: Migrate PrimeVue translations and complete English parity

**Files:**
- Create: `src/locales/primevue/zh-CN.json`
- Create: `src/locales/primevue/en-US.json`
- Modify: `src/locales/primevue.ts`

- [ ] **Step 1: Mechanically serialize the existing PrimeVue locale map**

```powershell
node --experimental-strip-types --input-type=module -e "import { mkdirSync, writeFileSync } from 'node:fs'; mkdirSync('src/locales/primevue', { recursive: true }); const locales = (await import('./src/locales/primevue.ts')).primeVueLocales; writeFileSync('src/locales/primevue/zh-CN.json', JSON.stringify(locales['zh-CN'], null, 2) + '\n', 'utf8'); writeFileSync('src/locales/primevue/en-US.json', JSON.stringify(locales['en-US'], null, 2) + '\n', 'utf8');"
```

- [ ] **Step 2: Add the missing English calendar resources**

Make `src/locales/primevue/en-US.json` structurally match Chinese by using this complete content:

```json
{
  "accept": "Yes",
  "reject": "No",
  "choose": "Choose",
  "upload": "Upload",
  "cancel": "Cancel",
  "dayNames": ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"],
  "dayNamesShort": ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"],
  "monthNames": ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"],
  "firstDayOfWeek": 0
}
```

- [ ] **Step 3: Replace PrimeVue translation literals with JSON imports**

Replace `src/locales/primevue.ts` with:

```ts
import enUS from './primevue/en-US.json'
import zhCN from './primevue/zh-CN.json'

export const primeVueLocales = {
  'zh-CN': zhCN,
  'en-US': enUS,
} as const
```

- [ ] **Step 4: Run parity and PrimeVue synchronization tests**

Run: `npm run test:run -- src/locales/__tests__/localeParity.spec.ts src/composables/__tests__/preferences.spec.ts`

Expected: both product and PrimeVue parity cases PASS; PrimeVue's accept label still changes between `Yes` and `确定`.

### Task 4: Remove legacy TypeScript resources and enforce the boundary

**Files:**
- Delete: `src/locales/zh-CN.ts`
- Delete: `src/locales/en-US.ts`
- Create: `src/locales/__tests__/localeResourceBoundary.spec.ts`

- [ ] **Step 1: Write the failing boundary test before deleting legacy files**

Create `src/locales/__tests__/localeResourceBoundary.spec.ts`:

```ts
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
```

- [ ] **Step 2: Run the boundary test and confirm it detects legacy files**

Run: `npm run test:run -- src/locales/__tests__/localeResourceBoundary.spec.ts`

Expected: FAIL because `zh-CN.ts` and `en-US.ts` still exist.

- [ ] **Step 3: Delete the legacy TypeScript language resources**

```powershell
git rm -- src/locales/zh-CN.ts src/locales/en-US.ts
```

- [ ] **Step 4: Run all locale-focused tests**

Run: `npm run test:run -- src/locales/__tests__/localeParity.spec.ts src/locales/__tests__/localeResourceBoundary.spec.ts src/locales/__tests__/noHardcodedChinese.spec.ts src/composables/__tests__/preferences.spec.ts`

Expected: all locale-focused tests PASS.

- [ ] **Step 5: Commit the migration as one atomic resource change**

```powershell
git add -- src/locales
git commit -m "refactor: move locale resources to JSON"
```

### Task 5: Full regression and browser verification

**Files:**
- Verify all files changed in Tasks 1–4.

- [ ] **Step 1: Run the complete automated suite**

Run: `npm run test:run`

Expected: the previous 60 tests plus the new parity and boundary tests all PASS.

- [ ] **Step 2: Run static and production-build verification**

Run: `npm run type-check`

Expected: PASS; TypeScript resolves all four JSON imports.

Run: `npm run build`

Expected: PASS; Vite bundles the JSON resources without new warnings.

- [ ] **Step 3: Inspect the resource boundary**

Run:

```powershell
rg -n "[\p{Han}]" src/locales --glob "*.ts"
rg --files src/locales
```

Expected: no Chinese translation literals in locale TypeScript wiring; resource listing contains four JSON files and no legacy `zh-CN.ts` or `en-US.ts`.

- [ ] **Step 4: Verify language switching in the running application**

Open `http://127.0.0.1:5174/#/settings`, switch between `简体中文` and `English`, then verify:

- Startup, navigation, tasks, settings, and PrimeVue controls update without reload.
- The chosen language persists after a browser refresh.
- No missing-key warnings or console errors appear.

- [ ] **Step 5: Review repository state**

Run: `git diff --check` and `git status --short`.

Expected: no whitespace errors and no uncommitted project changes.

