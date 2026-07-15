import { beforeEach, describe, expect, it, vi } from 'vitest'
import { mount } from '@vue/test-utils'
import RightContextPanel from '@/components/shell/RightContextPanel.vue'

describe('RightContextPanel', () => {
  beforeEach(() => vi.stubGlobal('matchMedia', vi.fn(() => ({ matches: true, addEventListener: vi.fn(), removeEventListener: vi.fn() }))))

  it('uses a drawer on compact windows', () => {
    const wrapper = mount(RightContextPanel, {
      props: { open: true },
      global: {
        mocks: { $t: (key: string) => key },
        stubs: { Drawer: { template: '<div data-testid="context-drawer"><slot /></div>' } },
      },
    })
    expect(wrapper.get('[data-testid="context-drawer"]').exists()).toBe(true)
  })
})
