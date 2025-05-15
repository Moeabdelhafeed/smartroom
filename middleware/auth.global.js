export default defineNuxtRouteMiddleware(async (to, from) => {
    if (import.meta.server) return
  
    const { isConnected, topic } = storeToRefs(useMqttStore())
    const {publish} = useMqttClient()

    if (isConnected.value) {
    
      publish(topic.value + "/command", 'door')
      publish(topic.value + "/command", 'gas/alarm')
      publish(topic.value + "/command", 'gas')
      publish(topic.value + "/command", 'dht')
      publish(topic.value + "/command", 'relay')
      console.log('MQTT connected successfully.')
    
    }



    // ✅ After connection logic, check route permissions
    if (!isConnected.value && to.path !== '/') {
      return navigateTo('/')
    }
  
    if (isConnected.value && to.path === '/') {
      return navigateTo('/dashboard')
    }
  })
  