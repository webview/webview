<script setup lang="ts">
import { init, type EChartsOption, type EChartsType } from 'echarts/core'
import { BarChart, LineChart, PieChart } from 'echarts/charts'
import { GridComponent, LegendComponent, TooltipComponent } from 'echarts/components'
import { CanvasRenderer } from 'echarts/renderers'
import { use } from 'echarts/core'
import { onBeforeUnmount, onMounted, ref, watch } from 'vue'

use([BarChart, LineChart, PieChart, GridComponent, LegendComponent, TooltipComponent, CanvasRenderer])

const props = withDefaults(defineProps<{
  option: EChartsOption
  summary: string
  height?: string
}>(), { height: '280px' })

const container = ref<HTMLDivElement>()
let chart: EChartsType | undefined
let observer: ResizeObserver | undefined

onMounted(() => {
  if (!container.value) return
  chart = init(container.value)
  chart.setOption(props.option)
  observer = new ResizeObserver(() => chart?.resize())
  observer.observe(container.value)
})

watch(() => props.option, (option) => chart?.setOption(option, true), { deep: true })

onBeforeUnmount(() => {
  observer?.disconnect()
  chart?.dispose()
})
</script>

<template>
  <div ref="container" role="img" :aria-label="summary" :style="{ height }" />
</template>
