<script setup lang="ts">
import { ref } from 'vue'
import { useRouter } from 'vue-router'
import Button from 'primevue/button'
import Checkbox from 'primevue/checkbox'
import InputText from 'primevue/inputtext'
import Password from 'primevue/password'

const router = useRouter()
const email = ref('demo@northstar.app')
const password = ref('northstar')
const remember = ref(true)
const loading = ref(false)

const submit = async () => {
  loading.value = true
  await new Promise((resolve) => window.setTimeout(resolve, 320))
  await router.push({ name: 'dashboard' })
}
</script>

<template>
  <section class="rounded-3xl border border-surface bg-surface-0 p-7 shadow-xl shadow-black/5 dark:bg-surface-900 sm:p-9">
    <div class="mb-8 flex items-center gap-3"><span class="grid size-11 place-items-center rounded-2xl bg-primary text-primary-contrast"><i class="pi pi-sparkles" /></span><strong class="text-lg">{{ $t('app.name') }}</strong></div>
    <h1 class="mb-2 mt-0 text-2xl font-medium tracking-tight">{{ $t('auth.welcome') }}</h1><p class="mb-7 mt-0 text-sm text-muted-color">{{ $t('auth.description') }}</p>
    <form class="grid gap-5" @submit.prevent="submit"><label class="form-field"><span>{{ $t('auth.email') }}</span><InputText v-model="email" type="email" autocomplete="email" required fluid /></label><label class="form-field"><span>{{ $t('auth.password') }}</span><Password v-model="password" :feedback="false" toggle-mask autocomplete="current-password" required fluid /></label><div class="flex items-center justify-between text-sm"><label class="flex items-center gap-2"><Checkbox v-model="remember" binary input-id="remember" />{{ $t('auth.remember') }}</label><a href="#" class="text-primary no-underline">{{ $t('auth.forgot') }}</a></div><Button type="submit" :label="$t('auth.login')" :loading="loading" class="mt-1" /></form><p class="mb-0 mt-6 text-center text-xs text-muted-color">{{ $t('auth.demo') }}</p>
  </section>
</template>
