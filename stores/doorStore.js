export const useDoorStore = defineStore('door', {
    state: () => ({ 
        isOpen: false,
        password: null,
    }),
    actions: {
        setIsOpen(value) {
            this.isOpen = value
        },
        setPassword(value) {
            this.password = value
        },
        clearPassword() {
            this.password = null
        }
       
    },
  })