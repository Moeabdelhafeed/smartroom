export const useMqttStore = defineStore('mqtt', {
    state: () => ({ 
        isConnected: false,
        error: null,
        topic: null,
        username: null,
        password: null,
        client: null,
    }),
    actions: {
        setIsConnected(value) {
            this.isConnected = value
        },
        setError(value) {
            this.error = value
        },
        setCredentials(username, password, topic) {
            this.username = username
            this.password = password
            this.topic = topic
        },
        setClient(client) {
            this.client = client
        },
        removeCredentials(){
            this.isConnected = false
            this.error = null
            this.topic = null
            this.username = null
            this.password = null
            this.client = null
        }
       
    },
  })