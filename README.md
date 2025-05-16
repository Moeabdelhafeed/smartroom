# SmartRoom Project

This is the official repository for the **SmartRoom** project.

✅ The project is **completed** and explained in detail in this Medium article: [Read the full article here](https://testing.com)

---

## ⚙️ Configuration Guide

To run this project with your own **HiveMQ Cloud** setup, follow these simple steps:

### 1. Set up your HiveMQ Cloud Cluster
- Create an account at [HiveMQ Cloud](https://www.hivemq.com/mqtt-cloud-broker/).
- Set up your cluster and note your **broker URL**, **port**, **username**, and **password**.

### 2. Configure the Arduino Code
Navigate to:
```
smartroom/smartroom.ino
```

Update the following values in your code:

```cpp
const char* ssid = "YOUR WIFI NAME";
const char* password = "YOUR WIFI PASSWORD";
const char* mqtt_server = "YOUR MQTT URL HIVEMQ";
const int mqtt_port = 8883;
const char* mqtt_username = "YOUR HIVEMQ CLOUD CREDENTIAL USERNAME";
const char* mqtt_password = "YOUR HIVEMQ CLOUD CREDENTIAL PASSWORD";
```

### 3. Configure the Nuxt App (MQTT Client)
Navigate to:
```
smartroom/compasables/useMqttClient.js
```

Change the MQTT URL line to use your own HiveMQ cloud instance:

```js
const url = `wss://{YOUR_CLOUD_URL}.s1.eu.hivemq.cloud:8884/mqtt`
```

Example:
```js
const url = `wss://f81fe19a2a0d48b69330dd2213f50a60.s1.eu.hivemq.cloud:8884/mqtt`
```

---

## ✅ Done!
That’s it! You’re now ready to use the SmartRoom project with your own HiveMQ Cloud setup.

If you run into issues, refer to the [Medium article](https://testing.com) for troubleshooting and additional insights.

---
