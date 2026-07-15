import { describe, expect, it } from 'vitest'
import { getAssistantReply } from '@/services/assistant'

describe('assistant service', () => {
  it('returns a deterministic local reply for browser previews', async () => {
    await expect(getAssistantReply('summarize')).resolves.toMatchObject({ role: 'assistant' })
  })
})
