<template>
    <div>
        <img src="/image.png" >
        <div class="container mx-auto p-4 relative">
        <div class="flex">
         <h1 class="text-3xl font-bold mb-5"> Smartroom</h1>
         <button @click="logout" class="btn btn-soft btn-error absolute end-0 me-4">
            log out
            </button>
            </div>
            
         <hr>
         <div class="grid grid-cols-2 gap-4 mt-8">

                <NuxtLink :to="'/dashboard/door'">

                <div class=" z-30 transition duration-300 transform bg-indigo-500 text-base text-white p-4 rounded-xl hover:bg-indigo-600 relative">
    <span v-if="alarm && isOpen" class="indicator-item badge badge-warning absolute top-0 right-0 translate-x-1/2 -translate-y-1/2">
        Warning 
    </span>
    door
</div>
                </NuxtLink>
                <NuxtLink :to="'/dashboard/gas'">
                
               <div class="transition duration-300 transform bg-indigo-500 text-base text-white p-4 rounded-xl hover:bg-indigo-600 relative">
    <span v-if="alarm" class="indicator-item badge badge-warning absolute top-0 right-0 translate-x-1/2 -translate-y-1/2">
        Warning
    </span>
    Gas
</div>


                </NuxtLink>
                <hr class="col-span-2 mt-5">
                
                
                
                
         </div>
         <div class="flex flex-row justify-around mt-4">

          <div  class=" w-50 text-center transition text-base duration-300 transform   text-white p-4 rounded-xl trans "  >
                    Humidity: {{ hum }}%
                </div>
                <div class="flex flex-col items-center">
                    
               <div
               @click="toggleFan"
  :class="{
    'bg-red-400 hover:bg-red-500 shadow-none': !isRelayOn,
    'animate-spin bg-green-400 hover:bg-green-500 shadow-none': isRelayOn
  }"
  class="w-12 cursor-pointer h-12 rounded-full flex items-center justify-center text-white transition duration-300 transform"
>
  <IconFan class="text-2xl" />
</div>
<input type="checkbox" @change="toggleGas($event.target.checked)" v-model="isGasRelayOn" class="toggle mt-3 toggle-secondary" />

<p v-if="isGasRelayOn"  class="absolute text-green-400 w-75 text-center bottom-[-43px]">The fan will work automatically on gas detection</p>
<p v-if="!isGasRelayOn" class=" text-red-400 absolute w-75 text-center bottom-[-43px]">The fan will not work automatically on gas detection</p>
                </div>


                 <div  class=" w-50  text-center text-base transition duration-300 transform   text-white p-4 rounded-xl trans "  >
                    Temperature: {{ temp }} &deg;C
                </div>
         </div>
        </div>
    </div>
</template>

<script setup>

const {disconnect} = useMqttClient()

const $router = useRouter()

const logout = () => {
    disconnect().then(() => $router.push('/')  )
}

const {hum, temp} = storeToRefs(useDhtStore())
const {alarm} = storeToRefs(useGasStore())

const {publish} = useMqttClient()

const {isOpen} = storeToRefs(useDoorStore())

const {topic} = storeToRefs(useMqttStore())


const {isRelayOn} =storeToRefs(useRelayStore()) 

const {isGasRelayOn} = storeToRefs(useGasRelayStore())

const toggleGas = (value) => {
  publish(topic.value, value ? 'gas/relay/on' : 'gas/relay/off');
}



const toggleFan = () => {
    publish(topic.value, isRelayOn.value ? 'relay/off' : 'relay/on')
}

</script>

<style scoped>

</style>