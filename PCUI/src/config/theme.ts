import { definePreset } from '@primeuix/themes'
import Aura from '@primeuix/themes/aura'

export const PcuiPreset = definePreset(Aura, {
  semantic: {
    primary: {
      50: '{indigo.50}',
      100: '{indigo.100}',
      200: '{indigo.200}',
      300: '{indigo.300}',
      400: '{indigo.400}',
      500: '{indigo.500}',
      600: '{indigo.600}',
      700: '{indigo.700}',
      800: '{indigo.800}',
      900: '{indigo.900}',
      950: '{indigo.950}',
    },
    colorScheme: {
      light: {
        surface: {
          0: '#ffffff',
          50: '#f8f9fc',
          100: '#f1f3f8',
          200: '#e6eaf2',
          300: '#d5dbe7',
          400: '#9aa5b8',
          500: '#647089',
          600: '#4a566d',
          700: '#354056',
          800: '#232c3e',
          900: '#172033',
          950: '#0d1423',
        },
      },
      dark: {
        surface: {
          0: '#ffffff',
          50: '#e9edf5',
          100: '#d5dbe7',
          200: '#b6c0d1',
          300: '#8e9bb2',
          400: '#687792',
          500: '#4c5a72',
          600: '#38465e',
          700: '#27344a',
          800: '#1b2638',
          900: '#111a2a',
          950: '#0b1220',
        },
      },
    },
  },
  components: {
    card: { root: { borderRadius: '14px' } },
    button: { root: { borderRadius: '10px' } },
    inputtext: { root: { borderRadius: '10px' } },
  },
})
