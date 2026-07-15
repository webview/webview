import { defineStore } from 'pinia'
import { ref } from 'vue'

export interface WorkspaceUser {
  id: string
  nameKey: string
  initialsKey: string
  email: string
  roleKey: string
}

export const useUserStore = defineStore('user', () => {
  const currentUser = ref<WorkspaceUser>({
    id: 'user-001',
    nameKey: 'user.name',
    initialsKey: 'user.initials',
    email: 'lin@northstar.app',
    roleKey: 'profile.designer',
  })
  return { currentUser }
})
