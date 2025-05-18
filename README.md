# Smart Room with ESP32 🔐🌡️💨

Welcome to the Smart Room project! This is a web-integrated smart room system powered by ESP32. It combines sensors, a web dashboard, and automation to create a safer, smarter environment.

## 🔗 Full Tutorial

👉 Check out the complete guide on Medium: [How to Build a Web-Integrated Smart Room with ESP32](https://medium.com/@almormohammad939/how-to-build-a-web-integrated-smart-room-with-esp32-1e6dcbfded6e)

This tutorial includes everything you need:
- Step-by-step hardware setup
- Wiring diagram
- Web dashboard integration using MQTT (HiveMQ)
- Full code walkthrough for ESP32 and Nuxt.js dashboard

## 🚀 Features

- **Keypad + LCD** access system
- **MQ-2 Gas Sensor** triggers fan & auto-unlock
- **Temperature & Humidity monitoring** with DHT11
- **Real-time Web Dashboard** with MQTT
- **Servo Motor door control**
- **Fan activation with Relay**

## 🧰 Components Used

- ESP32 Dev Board
- 3x4 Keypad
- LCD1602 (I2C)
- MQ-2 Gas Sensor
- DHT11 Sensor
- SG90 Servo Motor
- Relay Module (for Fan)
- LEDs, Jumper wires, Resistors, Breadboard

## 📦 Code & Dashboard

- ESP32 Code: `smartroom.ino`
- Web App (Nuxt.js): `/web-dashboard` folder

> Replace placeholders in code with your HiveMQ and WiFi credentials.

## 🛠️ Quick Start

```bash
# Web Dashboard Setup
npm install
npm run dev
```

```cpp
// Arduino Setup
// Update your WiFi and MQTT credentials
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";
```

## 📡 MQTT Integration (HiveMQ Cloud)

- Broker: HiveMQ Cloud
- Uses `PubSubClient` (ESP32) & Websockets (Web App)

## 🖥 Live Demo (Optional)

You can deploy the web dashboard to GitHub Pages easily.

## 🙌 Contribution

Feel free to fork, explore, or contribute ideas!

## 📄 License

MIT License

---

✨ Inspired and built with love using ESP32 + Nuxt + MQTT + Sensors
