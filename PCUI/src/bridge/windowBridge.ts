export type WindowBridgeMessage =
  | { type: 'window:minimize' }
  | { type: 'window:toggle-maximize' }
  | { type: 'window:close' }
  | { type: 'window:get-state' }

type WebViewMessageEvent = { data: unknown }

interface WebViewChannel {
  postMessage: (message: WindowBridgeMessage) => void
  addEventListener?: (type: 'message', listener: (event: WebViewMessageEvent) => void) => void
  removeEventListener?: (type: 'message', listener: (event: WebViewMessageEvent) => void) => void
}

export interface WindowLike {
  chrome?: { webview?: WebViewChannel }
}

export interface WindowBridge {
  minimize: () => Promise<void>
  toggleMaximize: () => Promise<void>
  close: () => Promise<void>
  isMaximized: () => Promise<boolean>
  onWindowStateChanged: (callback: (maximized: boolean) => void) => () => void
}

const isStateMessage = (value: unknown): value is { type: 'window:state'; maximized: boolean } =>
  typeof value === 'object' &&
  value !== null &&
  'type' in value &&
  value.type === 'window:state' &&
  'maximized' in value &&
  typeof value.maximized === 'boolean'

export const createWindowBridge = (target: WindowLike): WindowBridge => {
  const channel = target.chrome?.webview
  const post = (message: WindowBridgeMessage) => channel?.postMessage(message)

  return {
    minimize: async () => post({ type: 'window:minimize' }),
    toggleMaximize: async () => post({ type: 'window:toggle-maximize' }),
    close: async () => post({ type: 'window:close' }),
    isMaximized: async () => {
      if (!channel?.addEventListener) return false
      return new Promise<boolean>((resolve) => {
        let settled = false
        const finish = (value: boolean) => {
          if (settled) return
          settled = true
          channel.removeEventListener?.('message', listener)
          resolve(value)
        }
        const listener = (event: WebViewMessageEvent) => {
          if (isStateMessage(event.data)) finish(event.data.maximized)
        }
        channel.addEventListener?.('message', listener)
        globalThis.setTimeout(() => finish(false), 250)
        post({ type: 'window:get-state' })
      })
    },
    onWindowStateChanged: (callback) => {
      const listener = (event: WebViewMessageEvent) => {
        if (isStateMessage(event.data)) callback(event.data.maximized)
      }
      channel?.addEventListener?.('message', listener)
      return () => channel?.removeEventListener?.('message', listener)
    },
  }
}

export const windowBridge = createWindowBridge(window as Window & WindowLike)
