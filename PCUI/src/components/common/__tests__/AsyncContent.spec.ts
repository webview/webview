import { describe, expect, it } from 'vitest'
import { mount } from '@vue/test-utils'
import AsyncContent from '@/components/common/AsyncContent.vue'

const mountState = (props: Record<string, unknown>) => mount(AsyncContent, {
  props,
  slots: { default: '<div data-testid="content">Ready</div>' },
  global: { mocks: { $t: (key: string) => key } },
})

describe('AsyncContent', () => {
  it('renders loading state before content', () => {
    expect(mountState({ loading: true }).get('[data-testid="loading"]').exists()).toBe(true)
  })

  it('renders an error with a retry action', async () => {
    const wrapper = mountState({ error: 'Network unavailable' })

    await wrapper.get('[data-testid="retry"]').trigger('click')

    expect(wrapper.text()).toContain('Network unavailable')
    expect(wrapper.emitted('retry')).toHaveLength(1)
  })

  it('renders an empty state when requested', () => {
    expect(mountState({ empty: true }).get('[data-testid="empty"]').exists()).toBe(true)
  })

  it('renders content when the state is ready', () => {
    expect(mountState({}).get('[data-testid="content"]').text()).toBe('Ready')
  })
})
