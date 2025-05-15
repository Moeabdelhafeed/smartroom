export const useGasDoorStore = defineStore('gasDoor', {
    state: () => ({ 
        useGasDoor : false,
    }),
    actions: {
        setUseGasDoor(value) {
            this.useGasDoor = value
        },
    },
  })