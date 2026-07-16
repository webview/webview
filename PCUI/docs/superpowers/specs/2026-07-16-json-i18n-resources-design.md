# PCUI JSON 国际化资源迁移设计

日期：2026-07-16  
状态：待用户确认

## 1. 目标

将项目现有的 `src/locales/zh-CN.ts` 和 `src/locales/en-US.ts` 全量迁移为独立 JSON 语言资源，使 TypeScript 只负责 Vue I18n 初始化、语言选择和 PrimeVue 语言同步，不再承载界面翻译文案。

迁移后所有 Vue 组件继续通过 `t()` 或 `$t()` 获取文案，不改变页面调用方式、语言代码或本地持久化键。

## 2. 文件结构

采用“每种语言一个完整 JSON”结构：

```text
src/locales/
├─ index.ts
├─ primevue.ts
├─ messages/
│  ├─ zh-CN.json
│  └─ en-US.json
└─ __tests__/
   ├─ localeParity.spec.ts
   └─ noHardcodedChinese.spec.ts
```

选择该结构的原因：项目目前只有中文和英文，两份完整 JSON 容易浏览、搜索、交给翻译人员维护，也不会产生大量细碎文件。未来语言或功能数量显著增加时，可以在不改变组件调用键的前提下再拆成功能包或异步加载。

## 3. Vue I18n 加载方式

`src/locales/index.ts` 保留唯一的 I18n 实例，并静态导入两个 JSON：

```ts
import enUS from './messages/en-US.json'
import zhCN from './messages/zh-CN.json'

export const i18n = createI18n({
  legacy: false,
  locale: 'zh-CN',
  fallbackLocale: 'en-US',
  messages: { 'zh-CN': zhCN, 'en-US': enUS },
})
```

TypeScript/Vite 原生处理 JSON 模块，不引入额外国际化依赖或运行时网络请求。

## 4. 资源边界

- `messages/zh-CN.json` 和 `messages/en-US.json` 保存产品界面文案。
- `primevue.ts` 继续保存 PrimeVue 组件需要的语言配置，因为它包含库配置对象而非产品翻译页面结构。
- 组件、Store、Service、路由元信息继续只保存语言键，不保存可见中文界面文案。
- 导航和路由中的中文代码注释保留；它们不是运行时界面文案。
- 品牌名、错误码、键盘快捷键等不翻译内容可继续出现在组件或数据结构中。

## 5. 数据迁移

- 将两份 TypeScript 默认导出对象原样转换为合法 UTF-8 JSON。
- 保持全部既有键路径和文本内容不变，包括新增的 `startup.*`。
- JSON 使用双引号、无注释、无尾逗号。
- 完成迁移后删除 `zh-CN.ts` 和 `en-US.ts`，避免双份语言源产生漂移。
- 不改动 `pcui:locale` 持久化键、`AppLocale` 类型或 `zh-CN` / `en-US` 语言代码。

## 6. 一致性和错误防护

新增语言包结构一致性测试，递归收集两个 JSON 的所有叶子键并比较：

- 中文存在但英文缺失时测试失败。
- 英文存在但中文缺失时测试失败。
- 嵌套对象层级不同导致最终键不一致时测试失败。

现有 `noHardcodedChinese.spec.ts` 继续确保产品中文文案不会回流到组件和 TypeScript 逻辑；它仍忽略已确认的中文代码注释。

Vue I18n 继续使用英文作为缺失键回退语言。开发测试负责阻止缺失键进入主分支，运行时回退负责避免极端情况下显示空白。

## 7. 验收标准

- 项目中不再存在 `src/locales/zh-CN.ts` 和 `src/locales/en-US.ts`。
- 全部产品中英文文案位于 `src/locales/messages/*.json`。
- `src/locales/index.ts` 只加载 JSON 并配置 Vue I18n。
- 中文、英文语言包的叶子键集合完全一致。
- 启动页、工作台和设置页切换中英文时显示正确。
- PrimeVue 组件语言仍随应用语言同步。
- 原有 60 项测试保持通过，并新增语言包一致性测试。
- `npm run type-check` 和 `npm run build` 成功。

