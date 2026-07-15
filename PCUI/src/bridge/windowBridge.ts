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
      post({ type: 'window:get-state' })
      return false
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
