<template>
  <footer class="w-full fixed bottom-0 text-white py-4 mt-8">
    <div class="container mx-auto text-center text-sm">
      Built by Mohammad Abdelhafeed &copy; {{ new Date().getFullYear() }}
    </div>
  </footer>
</template>

<script setup>
// No script needed for static footer
</script>

<style scoped>
footer {
  letter-spacing: 0.5px;
}
</style>