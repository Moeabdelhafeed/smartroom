<template>
  <div class="min-h-screen flex flex-col">
    <!-- Header Section -->
    <div class="container mx-auto p-4">
      <div class="flex">
        <NuxtLink :to="'/dashboard'">
          <h1 class="text-3xl font-bold mb-5 cursor-pointer">
            <i class="bi bi-arrow-left"></i>
          </h1>
        </NuxtLink>
        <h1 class="text-3xl font-bold ms-5">Door</h1>
      </div>
      <hr>
    </div>

    <!-- Button Section (Grows) -->
  <div class="flex-1 flex-col flex items-center mt-30">
  <button
    @click="openDoor"
    :disabled="isOpen"
    :class="[
      'relative btn border-0 flex items-center justify-center transition duration-300 w-50 h-50 transform font-bold text-2xl text-white p-4 rounded-full',
      isOpen 
        ? 'bg-gray-500 shadow-none opacity-50 cursor-not-allowed'  // disabled style
        : 'bg-green-500 hover:shadow-lg hover:bg-green-600 cursor-pointer'
    ]"
  >
    <i class="bi bi-power text-6xl font-bold"></i>
  </button>
  

 <p v-if="isOpen && alarm && useGasDoor" class="text-warning text-lg mt-4">
  WARNING: Door is open! Gas is detected!
</p>
<p v-else-if="isOpen" class="text-white text-lg mt-4">
  Door will be closed automatically in: 3 sec
</p>

  <div class="flex flex-col items-center mt-8">
      <label class="mb-2 font-semibold">Open door automatically on gas detection</label>
      <input
        type="checkbox"
        @change="toggleGasDoor($event.target.checked)"
        v-model="useGasDoor"
        class="toggle toggle-secondary"
        :disabled="isOpen && doorOpenedManually"
      />
      <p v-if="useGasDoor" class="mt-2 text-green-400">
        The door will open automatically on gas detection
      </p>
      <p v-else class="mt-2 text-red-400">
        The door will not open automatically on gas detection
      </p>
    </div>
   <form @submit.prevent="setNewPassword" class="form-control   p-8 w-full max-w-sm 	 rounded-xl  mx-auto " >
       
        <label class="label">
            <span class="label-text">The new Password</span>
        </label>
      <input
  v-model="newPassword"
  required
  type="text"
  inputmode="numeric"
  pattern="[0-9]*"
  maxlength="8"
  placeholder="Enter numeric password"
  class="input input-bordered w-full max-w-xs"
/>

        
        <h1 class="text-center m-3 mb-0">Current password: {{ password }}</h1>
     

        <button type="submit"  :disabled="!newPassword "
        class="btn btn-primary mt-4 w-full">Set New Password</button>

     
        </form>
</div>



  </div>
</template>
<script setup>
const { isOpen, password } = storeToRefs(useDoorStore())
const { alarm } = storeToRefs(useGasStore())
const { useGasDoor } = storeToRefs(useGasDoorStore())

const newPassword = ref("")
const doorOpenedManually = ref(false)

const { publish } = useMqttClient()
const { topic } = storeToRefs(useMqttStore())
const onClickOpen = ref(true)

const openDoor = async () => {
  doorOpenedManually.value = true
  setTimeout(() => {
    onClickOpen.value = false
    doorOpenedManually.value = false // Reset after the door auto-closes
  }, 3500)
  publish(topic.value, 'door/open')
}

const toggleGasDoor = (value) => {
  publish(topic.value, value ? 'gas/door/on' : 'gas/door/off')
}

const setNewPassword = async () => {
  publish(topic.value, 'door/' + newPassword.value)
}
</script>

<style scoped>



</style>