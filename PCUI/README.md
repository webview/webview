# PCUI Desktop Workspace

一个面向 WebView2 的现代 Vue 3 桌面工作台基础工程。项目提供可伸缩侧栏、浅色/深色/跟随系统主题、中英文切换、WebView2 窗口控制桥接，以及工作台、项目、任务、文件、分析、团队、消息、日历、AI 助手、个人中心、设置、登录和异常页面。

## 技术栈

- Vue 3 + TypeScript + Vite
- PrimeVue 4 + Aura Design Tokens
- Tailwind CSS 4
- Pinia + Vue Router + Vue I18n
- ECharts
- Vitest + Vue Test Utils

## 开始使用

```bash
npm install
npm run dev
```

常用命令：

```bash
npm run test:run
npm run type-check
npm run build
npm run preview
```

## 目录结构

```text
src/
├─ bridge/        # WebView2 和浏览器降级桥接
├─ components/    # 壳层、公共组件和图表
├─ composables/   # 主题与国际化同步
├─ config/        # 导航与 PrimeVue 主题
├─ layouts/       # 默认布局和认证布局
├─ locales/       # zh-CN、en-US
├─ router/        # 路由与元数据
├─ services/      # 异步 mock 服务
├─ stores/        # Pinia 应用状态
├─ styles/        # Tailwind 和全局设计样式
├─ types/         # 共享 TypeScript 类型
└─ views/         # 页面模块
```

## 主题扩展

PrimeVue 的颜色和组件语义 Token 位于 `src/config/theme.ts`，页面布局变量位于 `src/styles/main.css`。主题状态支持 `light`、`dark` 和 `system`，统一使用根节点的 `.app-dark` 类驱动 PrimeVue 与 Tailwind。

新增颜色时优先扩展 Design Tokens 或全局语义变量，不要在各页面重复定义同一用途的颜色。

## 多语言扩展

语言资源位于 `src/locales/zh-CN.ts` 和 `src/locales/en-US.ts`。新增页面时：

1. 在两份资源中添加相同路径的翻译键。
2. 在 Vue 模板使用 `$t('path.key')`，在脚本中使用 `useI18n()`。
3. 执行 `npm run test:run`，本项目包含防止中文界面文案散落到 locale 外的回归测试。

语言切换会同步 Vue I18n、PrimeVue Locale 和 HTML `lang` 属性。

## WebView2 窗口桥接

前端只依赖 `src/bridge/windowBridge.ts`。默认通过 `window.chrome.webview.postMessage` 发送：

```text
window:minimize
window:toggle-maximize
window:close
window:get-state
```

宿主可通过 WebMessage 返回：

```json
{ "type": "window:state", "maximized": true }
```

普通浏览器环境会使用安全降级，不会因为没有 WebView2 而报错。顶部工具栏中带 `data-webview-drag-region` 的空白区域用于无边框窗口拖拽，交互按钮已经排除拖拽命中。

## 接入真实 API

页面不直接访问网络。将 `src/services/` 中的 mock 实现替换为真实请求即可，保持原有 Promise 返回类型，页面的加载、空数据和错误状态无需重写。
