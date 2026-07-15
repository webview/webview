import { mockApi } from './mockApi'

export interface AssistantMessage {
  id: number
  role: 'user' | 'assistant'
  content?: string
  contentKey?: string
}

export const getAssistantReply = (prompt: string) => mockApi<AssistantMessage>({
  id: Date.now(),
  role: 'assistant',
  contentKey: prompt.trim() ? 'assistantPage.localReply' : 'assistantPage.emptyReply',
}, 240)
