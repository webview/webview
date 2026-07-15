import { fileURLToPath, URL } from 'node:url'
import tailwindcss from '@tailwindcss/vite'
import vue from '@vitejs/plugin-vue'
import { defineConfig } from 'vite'

export default defineConfig({
  plugins: [vue(), tailwindcss()],
  resolve: {
    alias: {
      '@': fileURLToPath(new URL('./src', import.meta.url)),
    },
  },
  build: {
    // ECharts is loaded only by analytics routes; its tree-shaken vendor chunk is intentionally larger.
    chunkSizeWarningLimit: 650,
  },
  test: {
    environment: 'happy-dom',
    globals: true,
    include: ['src/**/*.spec.ts'],
  },
})
