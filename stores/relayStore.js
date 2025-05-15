export const useRelayStore = defineStore('relay', {
    state: () => ({ 
       isRelayOn: false,
    }),
    actions: {
        setRelayValue(value){
            this.isRelayOn = value
        }
    },
  })