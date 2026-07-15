import { describe, expect, it, vi } from 'vitest'
import { createWindowBridge } from '@/bridge/windowBridge'

describe('window bridge', () => {
  it('uses safe browser fallbacks when WebView2 is unavailable', async () => {
    const bridge = createWindowBridge({})

    await expect(bridge.minimize()).resolves.toBeUndefined()
    await expect(bridge.toggleMaximize()).resolves.toBeUndefined()
    await expect(bridge.close()).resolves.toBeUndefined()
    await expect(bridge.isMaximized()).resolves.toBe(false)
  })

  it('posts typed commands through the WebView2 channel', async () => {
    const postMessage = vi.fn()
    const bridge = createWindowBridge({
      chrome: { webview: { postMessage } },
    })

    await bridge.minimize()
    await bridge.toggleMaximize()
    await bridge.close()

    expect(postMessage).toHaveBeenNthCalledWith(1, { type: 'window:minimize' })
    expect(postMessage).toHaveBeenNthCalledWith(2, { type: 'window:toggle-maximize' })
    expect(postMessage).toHaveBeenNthCalledWith(3, { type: 'window:close' })
  })
})
