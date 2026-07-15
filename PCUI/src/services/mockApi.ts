export const mockApi = async <T>(data: T, delay = 160): Promise<T> => {
  await new Promise((resolve) => window.setTimeout(resolve, delay))
  return structuredClone(data)
}
