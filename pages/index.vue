<script setup>


const {connect} = useMqttClient()

const $router = useRouter()

const { isConnected, error } = storeToRefs(useMqttStore())

// Example values – in real app, bind these to form inputs
const mqttConfig = ref({
  username: '',
  password: '',
  topic: ''
})

const connectToMQTT = async () => {
        await connect(mqttConfig.value).then(() => {
            $router.push('/dashboard')
        })
  
}

definePageMeta({
  name: 'login'
})
</script>

<template>
  <div>
    <div
    v-if="error"
  role="alert"
  class="alert alert-error m-3 alert-soft fixed top-0 left-0 right-0 z-50 shadow-md justify-center"
>
  <span>{{ error }}</span>
</div>

    <form class="form-control  p-8 w-full max-w-sm 	 rounded-xl  mx-auto mt-10" @submit.prevent="connectToMQTT">
        <h1 class="text-3xl font-bold mb-6"> Smartroom</h1>
        <label class="label">
            <span class="label-text">Username</span>
        </label>
        <input  required v-model="mqttConfig.username" type="text" placeholder="Type here your username" class="input input-bordered w-full max-w-xs" />

        <label class="label mt-4">
            <span class="label-text">Password</span>
        </label>
        <input required v-model="mqttConfig.password" type="password" placeholder="Your password" class="input input-bordered w-full max-w-xs" />
        
        <label class="label mt-4">
            <span class="label-text">Your Esp ID</span>
        </label>
        <input required v-model="mqttConfig.topic" type="text" placeholder="example-1" class="input input-bordered w-full max-w-xs" />

        <button type="submit" :disabled="!mqttConfig.username || !mqttConfig.password || !mqttConfig.topic"
        class="btn btn-primary mt-4 w-full">Connect</button>
        </form>

  </div>
</template>
