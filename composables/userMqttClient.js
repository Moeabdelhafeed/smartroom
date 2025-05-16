import { ref } from 'vue'
import mqtt from 'mqtt'

export function useMqttClient() {
const $router = useRouter()
  const {isConnected, client} = storeToRefs(useMqttStore())  
  const {setIsConnected, removeCredentials, setError, setCredentials, setClient} = useMqttStore() 
  const {setIsOpen, clearPassword, setPassword} = useDoorStore()
  const {isOpen, password} = storeToRefs(useDoorStore()) 
  const {setIsOn, setAlarm} = useGasStore()
  const {isOn, alarm} = storeToRefs(useGasStore())
  const {setHum, setTemp } = useDhtStore()
  const {isRelayOn} = storeToRefs(useRelayStore())
  const {setRelayValue} = useRelayStore()
  const {isGasRelayOn} = storeToRefs(useGasRelayStore())
  const {setGasRelayValue} = useGasRelayStore()
  const {setUseGasDoor} = useGasDoorStore()
  const {useGasDoor} = storeToRefs(useGasDoorStore())
  

const connect = ({ username, password, topic }) => {
  return new Promise((resolve, reject) => {
    const url = `wss://{YOUR CLOUND URL HIVEMQ}:8884/mqtt` // Replace with your MQTT broker URL lik this: wss://{YOUR CLOUND URL HIVEMQ}:8884/mqtt
    
    const mqttClient = mqtt.connect(url, {
      username,
      password
    })

    mqttClient.on('connect', () => {
      console.log('MQTT connected successfully.')
      setIsConnected(true)
      mqttClient.subscribe(topic + "/response")
      mqttClient.publish(topic + "/command", 'door')
      mqttClient.publish(topic + "/command", 'gas/alarm')
      mqttClient.publish(topic + "/command", 'gas')
      mqttClient.publish(topic + "/command", 'dht')
      mqttClient.publish(topic + "/command", 'relay')
      mqttClient.publish(topic + "/command", 'gas/relay')
      mqttClient.publish(topic + "/command", 'gas/door')
      setCredentials(username, password, topic)
      setClient(mqttClient) 
      resolve()
    })

    mqttClient.on('message', (topic, message) => {
      const msg = { topic, message: message.toString() }
      if (msg.message === "door/open") {
       setIsOpen(true) 
      }else if (msg.message === "door/close") {
        setIsOpen(false)
      }else if (msg.message.startsWith("door/")) {
        let password = msg.message.substring(5); 
        setPassword(password)
}else if (msg.message === "gas/on") {
        setIsOn(true)
      }else if (msg.message === "gas/off") {
        setIsOn(false)
      }else if (msg.message === "gas/alarm/on") {
        setAlarm(true)
      }else if (msg.message === "gas/alarm/off") {
        setAlarm(false)
     }else if (msg.message.startsWith("dht/temp")) {
        let temp = msg.message.substring(9); 
        setTemp(temp)
     }else if (msg.message.startsWith("dht/hum")) {
        let hum = msg.message.substring(8); 
        setHum(hum)
     }else if (msg.message == "relay/on") {
        setRelayValue(true)
        isRelayOn.value = true
      }else if (msg.message == "relay/off") {
        setRelayValue(false)
        isRelayOn.value = false
      }else if (msg.message == "gas/relay/on") {
        setGasRelayValue(true)
        isGasRelayOn.value = true
      }else if (msg.message == "gas/relay/off") {
        setGasRelayValue(false)
        isGasRelayOn.value = false
      }else if (msg.message == "gas/door/on") {
        setUseGasDoor(true)
        useGasDoor.value = true;
      }else if (msg.message == "gas/door/off") {
        setUseGasDoor(false)
        useGasDoor.value = false;
      }
    })

    mqttClient.on('error', (err) => {
      removeCredentials()
      setError(err)
      $router.push('/')
      mqttClient.end()
      setTimeout(() => setError(null), 3000)
      reject(err)
    })

    mqttClient.on('close', () => {
       removeCredentials()
      console.log('MQTT connection closed.')
    })

    mqttClient.on('offline', () => {
       removeCredentials()
      console.log('MQTT client is offline.')
    })
  })
}

 const publish = (topic, message) => {
   console.log(client.value.connected)
  if (client.value && client.value.connected) {
    client.value.publish(topic + "/command", message)
    console.log(`Message "${message}" sent to topic "${topic}/command".`)
  } else {
    console.error('MQTT client is not connected. Message not sent.')
  }
}


 const disconnect = () => {
  return new Promise((resolve, reject) => {
    if (client.value && isConnected.value) {
      client.value.end(() => {
        removeCredentials()
        console.log('MQTT connection closed successfully.')
        resolve()
      })
    } else {
      console.log('No active MQTT connection to close.')
      resolve() // Still resolve if there's no active connection
    }
  })
}

  return {
    connect,
    publish,
    disconnect,

  }
}
