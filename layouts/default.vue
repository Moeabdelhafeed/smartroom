<template>
    <Html data-theme="synthwave">
        <div
  role="alert"
  v-if="alarm && $route.path !== '/dashboard/gas' && $route.name !== 'login'"
  class="alert alert-warning container mx-auto alert-soft fixed bottom-7 left-10 right-8 z-50 shadow-md flex justify-between items-center p-4"
>
  <span>Gas detected!</span>
  <NuxtLink :to="{name: 'Gas'}" class="btn btn-warning">Check now</NuxtLink>
</div>

        
        <div>
            <slot/>
        </div>
        <Footer />
    </Html>
</template>

<script setup>
const { alarm } = storeToRefs(useGasStore())
</script>

<style  scoped>

</style>