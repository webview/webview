import { computed, ref } from 'vue'
import { defineStore } from 'pinia'

export interface WorkspaceNotification {
  id: number
  titleKey: string
  read: boolean
}

export const useNotificationStore = defineStore('notifications', () => {
  const items = ref<WorkspaceNotification[]>([
    { id: 1, titleKey: 'notifications.review', read: false },
    { id: 2, titleKey: 'notifications.deadline', read: false },
    { id: 3, titleKey: 'notifications.welcome', read: true },
  ])
  const unreadCount = computed(() => items.value.filter((item) => !item.read).length)
  const markAllRead = () => items.value.forEach((item) => { item.read = true })
  return { items, unreadCount, markAllRead }
})
