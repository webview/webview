import { mockApi } from './mockApi'

export interface AssistantMessage {
  id: number
  role: 'user' | 'assistant'
  content: string
}

export const getAssistantReply = (prompt: string) => mockApi<AssistantMessage>({
  id: Date.now(),
  role: 'assistant',
  content: prompt.trim() ? '我已整理好这个请求。接入真实 AI 服务后，这里会返回模型生成的内容。' : '请输入你的问题。',
}, 240)
