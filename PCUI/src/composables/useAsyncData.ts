import { ref, shallowRef } from 'vue'

export const useAsyncData = <T>(loader: () => Promise<T>) => {
  const data = shallowRef<T>()
  const loading = ref(false)
  const error = ref<string>()
  const load = async () => {
    loading.value = true
    error.value = undefined
    try { data.value = await loader() }
    catch (reason) { error.value = reason instanceof Error ? reason.message : String(reason) }
    finally { loading.value = false }
  }
  return { data, loading, error, load }
}
