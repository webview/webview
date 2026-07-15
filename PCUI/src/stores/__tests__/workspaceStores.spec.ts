import { beforeEach, describe, expect, it } from 'vitest'
import { createPinia, setActivePinia } from 'pinia'
import { useNotificationStore } from '@/stores/notifications'
import { useUserStore } from '@/stores/user'

describe('workspace stores', () => {
  beforeEach(() => setActivePinia(createPinia()))

  it('exposes the current user independently from translations', () => {
    expect(useUserStore().currentUser.email).toBe('lin@northstar.app')
  })

  it('tracks unread notifications and marks them read', () => {
    const store = useNotificationStore()
    expect(store.unreadCount).toBe(2)
    store.markAllRead()
    expect(store.unreadCount).toBe(0)
  })
})
