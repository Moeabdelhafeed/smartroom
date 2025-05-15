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
        <h1 class="text-3xl font-bold ms-5">Gas</h1>
      </div>
      <hr>
    </div>

    <!-- Button Section (Grows) -->
  <div class="flex-1 flex-col flex items-center mt-30">
    
  <button
    @click="toggleGas"
    :class="[
      'relative btn border-0 flex items-center justify-center transition duration-300 w-50 h-50 transform font-bold text-2xl text-white p-4 rounded-full',
      !isOn 
        ? 'bg-gray-500 shadow-none opacity-50'  // disabled style
        : 'bg-green-500 hover:shadow-lg hover:bg-green-600 cursor-pointer'
    ]"
  >
    <i class="bi bi-power text-6xl font-bold"></i>
  </button>

   <p  class=" text-lg mt-4">
    {{ isOn ? 'Gas is ON' : 'Gas is OFF' }}
  </p>
  <p v-if="alarm"  class="text-warning text-lg mt-4">
    WARNING: Gas is detected!
  </p>
</div>



  </div>
</template>

<script setup>

const {isOn, alarm} = storeToRefs(useGasStore())



const {publish} = useMqttClient()
const {topic} = storeToRefs(useMqttStore())


definePageMeta({
  name: 'Gas'
})

const toggleGas = async () => {
    if (isOn.value) {
        publish(topic.value , 'gas/off')
    } else {
        publish(topic.value , 'gas/on')
    }
}



</script>

<style scoped>



</style>