import { defineStore } from 'pinia'
import { ref } from 'vue'

export interface WorkspaceUser {
  id: string
  name: string
  initials: string
  email: string
  roleKey: string
}

export const useUserStore = defineStore('user', () => {
  const currentUser = ref<WorkspaceUser>({
    id: 'user-001',
    name: 'Lin Qingyue',
    initials: 'L',
    email: 'lin@northstar.app',
    roleKey: 'profile.designer',
  })
  return { currentUser }
})
