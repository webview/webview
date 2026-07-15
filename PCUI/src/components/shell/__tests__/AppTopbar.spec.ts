import { afterEach, beforeEach, describe, expect, it } from 'vitest'
import { mount } from '@vue/test-utils'
import type { VueWrapper } from '@vue/test-utils'
import { createPinia, setActivePinia } from 'pinia'
import { createMemoryHistory, createRouter } from 'vue-router'
import AppTopbar from '@/components/shell/AppTopbar.vue'
import { i18n } from '@/locales'

const wrappers: VueWrapper[] = []
const mountTopbar = async () => {
  const pinia = createPinia()
  setActivePinia(pinia)
  const router = createRouter({
    history: createMemoryHistory(),
    routes: [
      { path: '/', name: 'dashboard', component: { template: '<div />' }, meta: { titleKey: 'navigation.dashboard' } },
      { path: '/projects', name: 'projects', component: { template: '<div />' } },
    ],
  })
  await router.push('/')
  await router.isReady()
  const wrapper = mount(AppTopbar, {
    attachTo: document.body,
    global: {
      plugins: [pinia, router, i18n],
      stubs: { Button: true, Menu: true, WindowControls: true, Tooltip: true },
      directives: { tooltip: () => {} },
    },
  })
  wrappers.push(wrapper)
  return wrapper
}

describe('AppTopbar global search', () => {
  beforeEach(() => {
    localStorage.clear()
    i18n.global.locale.value = 'zh-CN'
  })

  afterEach(() => {
    wrappers.splice(0).forEach((wrapper) => wrapper.unmount())
    document.body.innerHTML = ''
  })

  it('renders an editable search input', async () => {
    const wrapper = await mountTopbar()
    const search = wrapper.get('[aria-label="搜索任何内容"]')

    expect(search.element.tagName).toBe('INPUT')
    await search.setValue('项目')
    expect((search.element as HTMLInputElement).value).toBe('项目')
  })

  it('focuses the search input with Ctrl+K', async () => {
    const wrapper = await mountTopbar()
    const search = wrapper.get('[aria-label="搜索任何内容"]')

    document.dispatchEvent(new KeyboardEvent('keydown', { key: 'k', ctrlKey: true, cancelable: true }))

    expect(document.activeElement).toBe(search.element)
  })

  it('shows matching workspace pages while typing', async () => {
    const wrapper = await mountTopbar()
    const search = wrapper.get('[aria-label="搜索任何内容"]')

    await search.setValue('项目')

    expect(wrapper.get('[data-testid="global-search-results"]').text()).toContain('项目管理')
  })
})
