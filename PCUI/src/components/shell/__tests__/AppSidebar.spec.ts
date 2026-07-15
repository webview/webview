import { beforeEach, describe, expect, it } from 'vitest'
import { mount } from '@vue/test-utils'
import { createPinia, setActivePinia } from 'pinia'
import AppSidebar from '@/components/shell/AppSidebar.vue'
import { useAppStore } from '@/stores/app'

describe('AppSidebar', () => {
  beforeEach(() => {
    localStorage.clear()
    setActivePinia(createPinia())
  })

  it('collapses from the toggle control and keeps navigation accessible', async () => {
    const wrapper = mount(AppSidebar, {
      global: {
        plugins: [createPinia()],
        stubs: { RouterLink: { template: '<a><slot /></a>' }, Tooltip: true },
        mocks: { $t: (key: string) => key },
      },
    })
    const store = useAppStore()

    expect(wrapper.text()).toContain('navigation.dashboard')
    await wrapper.get('[data-testid="sidebar-toggle"]').trigger('click')

    expect(store.sidebarCollapsed).toBe(true)
    expect(wrapper.classes()).toContain('is-collapsed')
    expect(wrapper.get('nav').attributes('aria-label')).toBe('navigation.main')
  })
})
