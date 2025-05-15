export const useGasStore = defineStore('gas', {
    state: () => ({ 
       isOn: false,
       alarm: false,
    }),
    actions: {
        setIsOn(value) {
            this.isOn = value
        },
        setAlarm(value) {
            this.alarm = value
        },
       
    },
  })