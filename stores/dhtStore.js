export const useDhtStore = defineStore('dht', {
    state: () => ({ 
        hum: 0,
        temp: 0,
    }),
    actions: {
        setHum(value) {
            this.hum = parseFloat(value)
        },
        setTemp(value) {
            this.temp = parseFloat(value)
        }
    },
  })