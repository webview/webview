# PCUI Desktop Workspace Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 构建一个可由 WebView2 承载、支持主题与中英文切换、包含十二组标准页面的 Vue 3 + PrimeVue PC 应用框架。

**Architecture:** 应用使用 Vue Router 驱动布局与页面，Pinia 管理持久化偏好，PrimeVue 负责复杂交互组件，Tailwind CSS 负责布局和响应式。WebView2 能力封装在独立 bridge 中，普通浏览器使用安全降级实现；页面数据来自异步 mock services，便于替换为真实 API。

**Tech Stack:** Vue 3, TypeScript, Vite, PrimeVue 4, Tailwind CSS 4, Pinia, Vue Router, Vue I18n, ECharts, Vitest, Vue Test Utils

---

## File map

- `package.json`, `vite.config.ts`, `tsconfig*.json`: 构建、类型检查和测试配置。
- `src/main.ts`, `src/App.vue`: 应用启动与根视图。
- `src/styles/`: Tailwind 入口、设计变量、全局布局与 PrimeVue 协调样式。
- `src/config/`: PrimeVue preset、导航和应用常量。
- `src/router/`: 路由表与路由元数据类型。
- `src/stores/`: 应用偏好、用户和通知状态。
- `src/composables/`: 主题、国际化和响应式布局行为。
- `src/bridge/`: WebView2 窗口控制抽象与浏览器降级。
- `src/layouts/`: 默认桌面壳层与认证布局。
- `src/components/shell/`: 侧栏、顶部栏、窗口按钮和右侧上下文面板。
- `src/components/common/`: 页头、指标卡、状态和数据容器。
- `src/components/charts/`: ECharts 生命周期封装。
- `src/views/`: 十二组路由页面及各页面局部组件。
- `src/locales/`: 中文、英文资源和 PrimeVue locale 映射。
- `src/services/`: 异步 mock 数据与领域接口。
- `src/types/`: 跨模块共享类型。
- `src/**/__tests__/`: 状态、composable、bridge 与布局测试。

### Task 1: Scaffold and toolchain

**Files:**
- Create: `package.json`
- Create: `index.html`
- Create: `vite.config.ts`
- Create: `tsconfig.json`
- Create: `tsconfig.app.json`
- Create: `tsconfig.node.json`
- Create: `src/env.d.ts`
- Create: `src/main.ts`
- Create: `src/App.vue`
- Create: `src/styles/main.css`

- [ ] **Step 1: Create the package manifest**

Use Vue 3, PrimeVue 4, `@primeuix/themes`, Tailwind 4 and the listed routing/testing packages. Scripts must include `dev`, `build`, `type-check`, `test`, and `test:run`.

- [ ] **Step 2: Install dependencies**

Run: `npm install`

Expected: dependencies install and `package-lock.json` is created.

- [ ] **Step 3: Add Vite, TypeScript and Vitest configuration**

Configure `@vitejs/plugin-vue`, alias `@` to `src`, `happy-dom` test environment, globals, and `src/**/*.spec.ts` test inclusion.

- [ ] **Step 4: Add minimal boot files**

`src/main.ts` must create the Vue app and import `src/styles/main.css`. `App.vue` must render `<RouterView />`; router registration is added in Task 3.

- [ ] **Step 5: Verify the baseline**

Run: `npm run type-check`

Expected: PASS with no TypeScript errors.

- [ ] **Step 6: Commit**

Run: `git add PCUI && git commit -m "chore: scaffold Vue PCUI workspace"`

### Task 2: Theme, locale and WebView2 bridge primitives

**Files:**
- Create: `src/config/theme.ts`
- Create: `src/locales/index.ts`
- Create: `src/locales/zh-CN.ts`
- Create: `src/locales/en-US.ts`
- Create: `src/locales/primevue.ts`
- Create: `src/stores/app.ts`
- Create: `src/composables/useTheme.ts`
- Create: `src/composables/useLocale.ts`
- Create: `src/bridge/windowBridge.ts`
- Test: `src/stores/__tests__/app.spec.ts`
- Test: `src/bridge/__tests__/windowBridge.spec.ts`

- [ ] **Step 1: Write failing preference and bridge tests**

Tests must assert that the app store toggles sidebar state, persists theme/locale, and that browser bridge methods resolve safely when WebView2 is absent.

- [ ] **Step 2: Run tests to verify failure**

Run: `npm run test:run -- src/stores/__tests__/app.spec.ts src/bridge/__tests__/windowBridge.spec.ts`

Expected: FAIL because store and bridge modules do not exist.

- [ ] **Step 3: Implement the app store and bridge**

Define `ThemeMode = 'light' | 'dark' | 'system'` and `AppLocale = 'zh-CN' | 'en-US'`. `windowBridge` exposes `minimize`, `toggleMaximize`, `close`, `isMaximized`, and `onWindowStateChanged`, routing through `window.chrome.webview.postMessage` when present and returning safe defaults otherwise.

- [ ] **Step 4: Implement theme and locale composables**

`useTheme` must synchronize `.app-dark` on `document.documentElement` and react to `prefers-color-scheme`. `useLocale` must update Vue I18n locale, PrimeVue reactive locale, `document.documentElement.lang`, and persist the selection.

- [ ] **Step 5: Implement the PrimeVue preset**

Use `definePreset(Aura, ...)`, map primary tokens to indigo, use zinc-like surfaces in light mode and blue-gray surfaces in dark mode, and configure `darkModeSelector: '.app-dark'`.

- [ ] **Step 6: Run tests**

Run: `npm run test:run -- src/stores/__tests__/app.spec.ts src/bridge/__tests__/windowBridge.spec.ts`

Expected: PASS.

- [ ] **Step 7: Commit**

Run: `git add PCUI/src && git commit -m "feat: add theme locale and window bridge primitives"`

### Task 3: Router and standard application shell

**Files:**
- Create: `src/router/index.ts`
- Create: `src/router/routes.ts`
- Create: `src/types/navigation.ts`
- Create: `src/config/navigation.ts`
- Create: `src/layouts/DefaultLayout.vue`
- Create: `src/layouts/AuthLayout.vue`
- Create: `src/components/shell/AppSidebar.vue`
- Create: `src/components/shell/AppTopbar.vue`
- Create: `src/components/shell/WindowControls.vue`
- Create: `src/components/shell/RightContextPanel.vue`
- Test: `src/components/shell/__tests__/AppSidebar.spec.ts`

- [ ] **Step 1: Write the failing sidebar test**

Mount `AppSidebar` with Pinia and assert expanded labels are present, clicking the collapse control updates `appStore.sidebarCollapsed`, and the collapsed class is applied.

- [ ] **Step 2: Run the test to verify failure**

Run: `npm run test:run -- src/components/shell/__tests__/AppSidebar.spec.ts`

Expected: FAIL because the component does not exist.

- [ ] **Step 3: Implement navigation types and configuration**

Define typed navigation groups with translation keys, PrimeIcons icon names, route names, badges and optional permission identifiers. Configure all twelve route groups.

- [ ] **Step 4: Implement layouts and shell components**

The sidebar must support 240px and 72px modes. Topbar includes breadcrumbs, search, locale, theme, notification and user controls. `WindowControls` calls only `windowBridge` and shows hover danger styling on close. The topbar reserves a `data-webview-drag-region` empty area and keeps controls outside it.

- [ ] **Step 5: Implement typed lazy routes**

Add `DefaultLayout` routes for all application pages and `AuthLayout` routes for login/error pages. Route meta includes translation key, icon, breadcrumb and layout information.

- [ ] **Step 6: Register plugins in `main.ts`**

Register Pinia, Router, Vue I18n, PrimeVue with the custom preset, ToastService, ConfirmationService, and Tooltip directive.

- [ ] **Step 7: Run tests and type-check**

Run: `npm run test:run -- src/components/shell/__tests__/AppSidebar.spec.ts`

Run: `npm run type-check`

Expected: both PASS.

- [ ] **Step 8: Commit**

Run: `git add PCUI/src && git commit -m "feat: build responsive desktop application shell"`

### Task 4: Common UI components and mock services

**Files:**
- Create: `src/components/common/PageHeader.vue`
- Create: `src/components/common/MetricCard.vue`
- Create: `src/components/common/AsyncContent.vue`
- Create: `src/components/common/StatusBadge.vue`
- Create: `src/components/charts/BaseChart.vue`
- Create: `src/services/mockApi.ts`
- Create: `src/services/dashboard.ts`
- Create: `src/types/dashboard.ts`
- Test: `src/components/common/__tests__/AsyncContent.spec.ts`

- [ ] **Step 1: Write failing async-state tests**

Test loading skeleton, error with retry button, empty state, and default slot rendering.

- [ ] **Step 2: Run the test to verify failure**

Run: `npm run test:run -- src/components/common/__tests__/AsyncContent.spec.ts`

Expected: FAIL because `AsyncContent.vue` does not exist.

- [ ] **Step 3: Implement common components**

Use PrimeVue Skeleton, Message, Button, Tag and Card where appropriate. Each component must expose typed props and slots, and all visible strings must use translation keys or caller-provided translated text.

- [ ] **Step 4: Implement ECharts lifecycle wrapper**

Initialize on mount, resize through `ResizeObserver`, update options through a watcher, dispose on unmount, and expose an accessible text summary.

- [ ] **Step 5: Implement asynchronous mock services**

`mockApi` returns Promises with a short deterministic delay. Dashboard service returns typed metrics, progress series, tasks and recent projects.

- [ ] **Step 6: Run tests**

Run: `npm run test:run -- src/components/common/__tests__/AsyncContent.spec.ts`

Expected: PASS.

- [ ] **Step 7: Commit**

Run: `git add PCUI/src && git commit -m "feat: add shared workspace components and services"`

### Task 5: Dashboard and productivity pages

**Files:**
- Create: `src/views/dashboard/DashboardView.vue`
- Create: `src/views/projects/ProjectsView.vue`
- Create: `src/views/tasks/TasksView.vue`
- Create: `src/views/files/FilesView.vue`
- Create: `src/views/analytics/AnalyticsView.vue`
- Create: `src/views/calendar/CalendarView.vue`
- Create: `src/views/*/components/*.vue` as page-specific focused components

- [ ] **Step 1: Implement the dashboard**

Compose PageHeader, four MetricCards, BaseChart, task list and recent project list. Load through the dashboard service and show loading/error/empty states through AsyncContent.

- [ ] **Step 2: Implement project and task pages**

Projects uses PrimeVue DataTable with filters plus card/list toggle and detail Drawer. Tasks uses PrimeVue Tabs to switch between DataTable and status board columns.

- [ ] **Step 3: Implement file and calendar pages**

Files uses Tree for folders, DataTable for files and FileUpload for upload state. Calendar provides a month grid, upcoming list and Dialog-based schedule editor without adding a second calendar library.

- [ ] **Step 4: Implement analytics**

Compose date range controls, metric cards, two BaseChart instances and a PrimeVue DataTable detail section.

- [ ] **Step 5: Verify routes**

Run: `npm run type-check`

Run: `npm run build`

Expected: both PASS and all six routes compile as lazy chunks.

- [ ] **Step 6: Commit**

Run: `git add PCUI/src && git commit -m "feat: add dashboard and productivity pages"`

### Task 6: Collaboration, assistant and settings pages

**Files:**
- Create: `src/views/team/TeamView.vue`
- Create: `src/views/messages/MessagesView.vue`
- Create: `src/views/assistant/AssistantView.vue`
- Create: `src/views/profile/ProfileView.vue`
- Create: `src/views/settings/SettingsView.vue`
- Create: `src/views/*/components/*.vue` as page-specific focused components

- [ ] **Step 1: Implement team and messages pages**

Team uses DataTable, AvatarGroup, role tags and an invite Dialog. Messages uses category navigation, message list, detail content and read/unread state.

- [ ] **Step 2: Implement the AI assistant page**

Build a conversation sidebar, message area, starter prompts and input composer. Keep responses local and deterministic; expose a service boundary for later API replacement.

- [ ] **Step 3: Implement profile and settings**

Profile uses standard form sections and validation-ready fields. Settings controls appearance, locale, notifications and keyboard shortcuts through app store state.

- [ ] **Step 4: Verify pages**

Run: `npm run type-check`

Run: `npm run build`

Expected: both PASS.

- [ ] **Step 5: Commit**

Run: `git add PCUI/src && git commit -m "feat: add collaboration assistant and settings pages"`

### Task 7: Authentication and error states

**Files:**
- Create: `src/views/auth/LoginView.vue`
- Create: `src/views/errors/NotFoundView.vue`
- Create: `src/views/errors/ForbiddenView.vue`

- [ ] **Step 1: Implement login view**

Use AuthLayout, PrimeVue form controls, clear validation states, remember-me option and a mock sign-in action that routes to dashboard.

- [ ] **Step 2: Implement 404 and forbidden views**

Provide concise localized explanations and safe navigation actions. Use the same brand system without the application sidebar.

- [ ] **Step 3: Verify routes and build**

Run: `npm run type-check && npm run build`

Expected: PASS.

- [ ] **Step 4: Commit**

Run: `git add PCUI/src && git commit -m "feat: add authentication and error pages"`

### Task 8: Final QA and documentation

**Files:**
- Create: `README.md`
- Modify: translations and styles discovered during QA

- [ ] **Step 1: Complete translations**

Search Vue and TypeScript source for user-visible Chinese/English literals outside locale files. Move reusable interface text into matching `zh-CN` and `en-US` keys.

- [ ] **Step 2: Add project documentation**

Document requirements, install commands, development/build commands, structure, theme extension, locale extension and the WebView2 bridge contract.

- [ ] **Step 3: Run the complete verification suite**

Run: `npm run test:run`

Run: `npm run type-check`

Run: `npm run build`

Expected: all commands PASS.

- [ ] **Step 4: Run responsive visual QA**

Open the local app at 1440px, 1280px and 1024px widths. Verify sidebar transitions, window buttons, theme switch, locale switch, overlays, tables and no horizontal overflow.

- [ ] **Step 5: Commit final QA changes**

Run: `git add PCUI && git commit -m "docs: finish PCUI workspace foundation"`
