export const useGasRelayStore = defineStore('gasRelay', {
    state: () => ({ 
        isGasRelayOn: false,
    }),
    actions: {
        setGasRelayValue(value){
            this.isGasRelayOn = value
        }
    },
  })