# PCUI Typography System Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Apply a readable, fixed-scale typography system for the bilingual WebView2 desktop workspace and verify it in code and a real browser.

**Architecture:** Define semantic font tokens and the Windows-first bilingual font stack in `main.css`, then consume those tokens through semantic component classes and PrimeVue selectors. Keep page structure and brand colors unchanged. Verify the contract with a CSS regression test plus the existing multi-route browser QA.

**Tech Stack:** Vue 3, TypeScript, PrimeVue 4, Tailwind CSS 4, Vitest, Playwright

---

### Task 1: Define the global typography contract

**Files:**
- Create: `src/styles/__tests__/typography.spec.ts`
- Modify: `src/styles/main.css`

- [ ] **Step 1: Write the failing token test**

```ts
import { describe, expect, it } from 'vitest'
import { readFileSync } from 'node:fs'

const css = readFileSync(new URL('../main.css', import.meta.url), 'utf8')

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
```

- [ ] **Step 2: Run the test and verify RED**

Run: `npm test -- --run src/styles/__tests__/typography.spec.ts`

Expected: FAIL because the semantic tokens and Windows bilingual stack are absent.

- [ ] **Step 3: Add the semantic tokens and base rendering rules**

Add to `:root` in `src/styles/main.css`:

```css
--font-sans: "Segoe UI Variable", "Segoe UI", "Microsoft YaHei UI", "PingFang SC", system-ui, -apple-system, BlinkMacSystemFont, sans-serif;
--text-caption: 0.75rem;
--leading-caption: 1.125rem;
--text-ui: 0.875rem;
--leading-ui: 1.25rem;
--text-body: 1rem;
--leading-body: 1.5rem;
--text-section: 1.25rem;
--leading-section: 1.75rem;
--text-page: 1.75rem;
--leading-page: 2.25rem;
--text-metric: 1.5rem;
--leading-metric: 1.875rem;
--weight-regular: 400;
--weight-medium: 500;
--weight-semibold: 600;
```

Set `font-family: var(--font-sans)`, `font-size: var(--text-body)`, `line-height: var(--leading-body)`, `font-kerning: normal`, and `font-synthesis: none` on the root document.

- [ ] **Step 4: Run the token test and verify GREEN**

Run: `npm test -- --run src/styles/__tests__/typography.spec.ts`

Expected: PASS.

- [ ] **Step 5: Commit the contract**

```powershell
git add PCUI/src/styles/main.css PCUI/src/styles/__tests__/typography.spec.ts
git commit -m "style: define PCUI typography tokens"
```

### Task 2: Apply semantic hierarchy to shared components

**Files:**
- Modify: `src/components/common/PageHeader.vue`
- Modify: `src/components/common/MetricCard.vue`
- Modify: `src/styles/main.css`
- Test: `src/styles/__tests__/typography.spec.ts`

- [ ] **Step 1: Extend the test with shared component contracts**

```ts
it('maps shared component roles to semantic tokens', () => {
  expect(css).toContain('.page-title')
  expect(css).toContain('font-size: var(--text-page)')
  expect(css).toContain('.page-description')
  expect(css).toContain('.metric-value')
  expect(css).toContain('font-variant-numeric: tabular-nums')
})
```

- [ ] **Step 2: Run the test and verify RED**

Run: `npm test -- --run src/styles/__tests__/typography.spec.ts`

Expected: FAIL because the semantic component selectors do not exist.

- [ ] **Step 3: Replace utility-only typography with semantic classes**

Use these classes in `PageHeader.vue`:

```vue
<p v-if="eyebrow" class="page-eyebrow">{{ eyebrow }}</p>
<h1 class="page-title">{{ title }}</h1>
<p v-if="description" class="page-description">{{ description }}</p>
```

Use these classes in `MetricCard.vue`:

```vue
<span class="metric-label">{{ label }}</span>
<strong class="metric-value">{{ value }}</strong>
<small v-if="helper" class="metric-helper">{{ helper }}</small>
```

Define each class in `main.css` with the approved size, line-height, weight, spacing, and tabular numerals for metrics.

```css
.page-eyebrow { margin: 0 0 .25rem; color: var(--p-text-muted-color); font-size: var(--text-ui); line-height: var(--leading-ui); }
.page-title { margin: 0; font-size: var(--text-page); line-height: var(--leading-page); font-weight: var(--weight-semibold); letter-spacing: -.02em; text-wrap: balance; }
.page-description { max-width: 68ch; margin: .375rem 0 0; color: var(--p-text-muted-color); font-size: var(--text-ui); line-height: var(--leading-ui); text-wrap: pretty; }
.metric-label { overflow: hidden; color: var(--p-text-muted-color); font-size: var(--text-ui); line-height: var(--leading-ui); text-overflow: ellipsis; white-space: nowrap; }
.metric-value { overflow: hidden; font-size: var(--text-metric); line-height: var(--leading-metric); font-weight: var(--weight-semibold); letter-spacing: -.015em; font-variant-numeric: tabular-nums; text-overflow: ellipsis; white-space: nowrap; }
.metric-helper { overflow: hidden; color: var(--p-text-muted-color); font-size: var(--text-caption); line-height: var(--leading-caption); text-overflow: ellipsis; white-space: nowrap; }
```

- [ ] **Step 4: Run the test and verify GREEN**

Run: `npm test -- --run src/styles/__tests__/typography.spec.ts`

Expected: PASS.

- [ ] **Step 5: Commit shared hierarchy**

```powershell
git add PCUI/src/components/common/PageHeader.vue PCUI/src/components/common/MetricCard.vue PCUI/src/styles/main.css PCUI/src/styles/__tests__/typography.spec.ts
git commit -m "style: apply semantic text hierarchy"
```

### Task 3: Normalize shell and PrimeVue component density

**Files:**
- Modify: `src/styles/main.css`
- Test: `src/styles/__tests__/typography.spec.ts`

- [ ] **Step 1: Add a failing UI density contract**

```ts
it('uses the UI text role for shell and PrimeVue controls', () => {
  expect(css).toContain('.sidebar-link')
  expect(css).toContain('.p-button')
  expect(css).toContain('.p-datatable')
  expect(css).toContain('font-size: var(--text-ui)')
  expect(css).toContain('.panel-heading h2')
  expect(css).toContain('font-size: var(--text-section)')
})
```

- [ ] **Step 2: Run the test and verify RED**

Run: `npm test -- --run src/styles/__tests__/typography.spec.ts`

Expected: FAIL because the PrimeVue and panel mappings are absent.

- [ ] **Step 3: Add scoped product typography mappings**

Add these scoped mappings to `main.css`:

```css
.sidebar-link,.app-topbar,.top-search,.global-search-result,
.p-button,.p-inputtext,.p-select-label,.p-tab,
.p-datatable .p-datatable-thead > tr > th,
.p-datatable .p-datatable-tbody > tr > td,
.form-field,.setting-row { font-size: var(--text-ui); line-height: var(--leading-ui); }

.p-button,.p-datatable .p-datatable-thead > tr > th,.form-field > span,.setting-row strong { font-weight: var(--weight-medium); }
.p-datatable .p-datatable-tbody > tr > td { font-weight: var(--weight-regular); font-variant-numeric: tabular-nums; }
.p-datatable .p-datatable-tbody strong { font-weight: var(--weight-medium); }
.panel-heading h2 { margin: 0; font-size: var(--text-section); line-height: var(--leading-section); font-weight: var(--weight-semibold); letter-spacing: -.01em; }
.panel-heading p { margin: .25rem 0 0; color: var(--p-text-muted-color); font-size: var(--text-ui); line-height: var(--leading-ui); }
.p-dialog-title { font-size: var(--text-section); line-height: var(--leading-section); font-weight: var(--weight-semibold); }
.message-copy,.assistant-copy { font-size: var(--text-body); line-height: var(--leading-body); }
```

- [ ] **Step 4: Run the typography and full test suites**

Run: `npm test -- --run src/styles/__tests__/typography.spec.ts`

Expected: PASS.

Run: `npm test -- --run`

Expected: all tests pass.

- [ ] **Step 5: Commit normalized density**

```powershell
git add PCUI/src/styles/main.css PCUI/src/styles/__tests__/typography.spec.ts
git commit -m "style: normalize desktop UI type density"
```

### Task 4: Verify rendered typography and responsive behavior

**Files:**
- Modify: `scripts/qa_app.py`

- [ ] **Step 1: Add browser assertions for rendered sizes**

After the dashboard loads, read computed styles and assert:

```python
assert page.locator(".page-title").evaluate("el => getComputedStyle(el).fontSize") == "28px"
assert page.locator(".page-description").evaluate("el => getComputedStyle(el).fontSize") == "14px"
assert page.locator(".sidebar-link").first.evaluate("el => getComputedStyle(el).fontSize") == "14px"
```

On the files route assert table cells use `14px`, then keep the existing 1440/1280/1024 overflow and route checks.

- [ ] **Step 2: Run browser QA**

Run: `python scripts/qa_app.py`

Expected: exit code 0, no browser console errors, no horizontal overflow.

- [ ] **Step 3: Run final verification**

Run: `npm run type-check`

Expected: PASS.

Run: `npm run build`

Expected: Vite production build completes successfully.

Run: `git diff --check`

Expected: no whitespace errors.

- [ ] **Step 4: Commit browser verification**

```powershell
git add PCUI/scripts/qa_app.py
git commit -m "test: verify rendered typography scale"
```
