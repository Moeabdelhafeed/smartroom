#include <ESP32Servo.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Preferences.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <WiFiClientSecure.h>

// --- Servo setup ---
Servo myServo;
const int servoPin = 13;

const int GAS_PIN = 32;
const int LED_PIN = 12; 
const int GAS_LED_PIN = 14;

const int DHT_PIN = 27;
DHT dht(DHT_PIN, DHT11);


unsigned long lastGasCheck = 0;
const unsigned long gasCheckInterval = 1000;

unsigned long previousMillis = 0;
const long interval = 1000; // 1 second

bool gasPreviouslyDetected = false;
bool doorOpenedDueToGas = false;
bool isOn = false;
bool isRelayOn = false;
bool useGasRelay = false;
bool useGasDoor = false;

const int RELAY_PIN = 25;
int gas = 0;

// --- LCD setup ---
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Use 0x3F if needed

// --- Preferences for storing password ---
Preferences preferences;
String storedPassword = "1234";

// --- Keypad setup ---
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {19, 18, 5, 17};
byte colPins[COLS] = {16, 4, 0, 2};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --- MQTT setup ---
const char* ssid = "YOUR WIFI NAME";
const char* password = "YOUR WIFI PASSWORD";
const char* mqtt_server = "YOUR MQTT URL HIVEMQ";
const int mqtt_port = 8883;
const char* mqtt_username = "YOUR HIVEMQ CLOUD CREDENTIAL USERNAME";
const char* mqtt_password = "YOUR HIVEMQ CLOUD CREDENTIAL PASSWORD";
const char* mqtt_command_topic = "esp32-1/command";
const char* mqtt_response_topic = "esp32-1/response";


WiFiClientSecure espClient;
PubSubClient client(espClient);

// --- State ---
String enteredPassword = "";
bool isUnlocked = false;

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(GAS_PIN, INPUT);
  pinMode(GAS_LED_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  setup_wifi();
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqtt_callback);

  myServo.attach(servoPin);
  lockDoor();

  Wire.begin(21,22);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password");

  preferences.begin("doorlock", false);
  storedPassword = preferences.getString("password", storedPassword);
  isOn = preferences.getBool("isOn", true);
  isRelayOn = preferences.getBool("isRelayOn", true);
  useGasRelay = preferences.getBool("useGasRelay", true);
  useGasDoor = preferences.getBool("useGasDoor", true);
  digitalWrite(RELAY_PIN, isRelayOn ? LOW : HIGH);

  // Send all current states to the web
  send_message("door/" + storedPassword);
  send_message(isOn ? "gas/on" : "gas/off");
  send_message(isRelayOn ? "relay/on" : "relay/off");
  send_message(useGasRelay ? "gas/relay/on" : "gas/relay/off");
  send_message(useGasDoor ? "gas/door/on" : "gas/door/off");
  if (isRelayOn){
    digitalWrite(RELAY_PIN, HIGH);
  }else{
    digitalWrite(RELAY_PIN, LOW);
    
  }

  int gasStatus = digitalRead(GAS_PIN);
  if (gasStatus == LOW) {
    send_message("gas/alarm/on");
    if (useGasRelay && isOn) {
      isRelayOn = true;
      preferences.putBool("isRelayOn", true);
      digitalWrite(RELAY_PIN, HIGH);
      send_message("relay/on");
    }
    if (useGasDoor && isOn && !isUnlocked) {
      unlockDoor();
      doorOpenedDueToGas = true;
      send_message("door/open");
    }else {
  send_message("gas/alarm/off");
  if (isUnlocked && doorOpenedDueToGas) {
    lockDoor();
    doorOpenedDueToGas = false;
    send_message("door/close");
  }
}
  } else {
    send_message("gas/alarm/off");
    if (useGasRelay && isOn) {
      isRelayOn = false;
      preferences.putBool("isRelayOn", false);
      digitalWrite(RELAY_PIN, LOW);
      send_message("relay/off");
    }
    if (useGasDoor && isOn && isUnlocked && doorOpenedDueToGas) {
      lockDoor();
      doorOpenedDueToGas = false;
      send_message("door/close");
    }
  }

  // Optionally, send DHT sensor values at startup
 readDHT();
}



void loop() {
  if (!client.connected()) reconnect();
  client.loop();

 readDHT();


  char key = keypad.getKey();
  if (key) {
    lcd.setCursor(0, 1);
    if (key == '*') {
      enteredPassword = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Password");
    } else if (key == '#') {
      checkPassword();
    } else if (enteredPassword.length() < 8) {
      enteredPassword += key;
      lcd.setCursor(enteredPassword.length() - 1, 1);
      lcd.print("*");
    }
  }

  if (isOn && (millis() - lastGasCheck >= gasCheckInterval)) {
    lastGasCheck = millis();
    checkGasSensor();
  }
}

void readDHT() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    send_message("dht/temp/" + String(temperature));
    send_message("dht/hum/" + String(humidity));

  }
}

void checkGasSensor() {
  if (!isOn) {
    gasPreviouslyDetected = false;
    digitalWrite(LED_PIN, LOW);
    digitalWrite(GAS_LED_PIN, LOW);
    if (doorOpenedDueToGas) {
      doorOpenedDueToGas = false;
      lockDoor();
      send_message("door/close");
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Password");
    return;
  }

  int gasStatus = digitalRead(GAS_PIN);

  if (gasStatus == LOW) { // gas detected
    if (!gasPreviouslyDetected) {
      gasPreviouslyDetected = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("GAS DETECTED!!");
      doorOpenedDueToGas = true;
     
      digitalWrite(GAS_LED_PIN, HIGH);

      send_message("gas/alarm/on");

      // Only open door if both useGasDoor and isOn are true
      if (useGasDoor && isOn) {
        unlockDoor();
         digitalWrite(LED_PIN, HIGH);
        send_message("door/open");
      }

      // Only activate relay if both useGasRelay and isOn are true
      if (useGasRelay && isOn) {
        isRelayOn = true;
        preferences.putBool("isRelayOn", true);
        digitalWrite(RELAY_PIN, HIGH);
        send_message("relay/on");
      }

      Serial.println("⚠️ Gas detected (digital alert)");
    }
  } else { // gas not detected
    if (gasPreviouslyDetected) {
      gasPreviouslyDetected = false;
      if (doorOpenedDueToGas) {
        doorOpenedDueToGas = false;
        lockDoor();
         digitalWrite(LED_PIN, LOW);
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Password");
      digitalWrite(LED_PIN, LOW);
      digitalWrite(GAS_LED_PIN, LOW);
      send_message("gas/alarm/off");

      // Turn off relay when gas is gone
      if (useGasRelay && isOn) {
        isRelayOn = false;
        preferences.putBool("isRelayOn", false);
        digitalWrite(RELAY_PIN, LOW);
        send_message("relay/off");
      }

      Serial.println("✅ Gas levels normal");
    }
  }
}

void checkPassword() {
  if (enteredPassword == storedPassword) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");
    unlockDoor();
    digitalWrite(LED_PIN, HIGH);
    delay(3000);
    digitalWrite(LED_PIN, LOW);
    lockDoor();
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied");
    digitalWrite(LED_PIN, LOW);
    lockDoor();
  }

  enteredPassword = "";
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password");
  
}

void lockDoor() {
  myServo.write(0);
  isUnlocked = false;
}

void unlockDoor() {
  myServo.write(90);
  isUnlocked = true;
}

void setNewPassword(String newPass) {
  storedPassword = newPass;
  preferences.putString("password", storedPassword);
}

void setup_wifi() {
  delay(10);
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "esp32-1";
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      client.subscribe(mqtt_command_topic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println("Message received: " + message);

  if (message == "door/open") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");
    digitalWrite(LED_PIN, HIGH);
    unlockDoor();
    send_message("door/open");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied");
    digitalWrite(LED_PIN, LOW);
    lockDoor();
    send_message("door/close");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Password");

  } else if (message.startsWith("door/")) {
    String newPass = message.substring(5);
    setNewPassword(newPass);
    send_message("door/" + newPass);

  } else if (message == "door") {
    String pass = preferences.getString("password", storedPassword);
    send_message("door/" + pass);

  } else if (message == "gas/on") {
    isOn = true;
    preferences.putBool("isOn", true);
    send_message("gas/on");

    int gasStatus = digitalRead(GAS_PIN);
    if (gasStatus == LOW) {
      send_message("gas/alarm/on");
      if (useGasRelay) {
        isRelayOn = true;
        preferences.putBool("isRelayOn", true);
        digitalWrite(RELAY_PIN, HIGH);
        send_message("relay/on");
      }
    } else {
      send_message("gas/alarm/off");
      if (useGasRelay) {
        isRelayOn = false;
        preferences.putBool("isRelayOn", false);
        digitalWrite(RELAY_PIN, LOW);
        send_message("relay/off");
      }
    }

    checkGasSensor();

  } else if (message == "gas/off") {
    isOn = false;
    preferences.putBool("isOn", false);
    send_message("gas/off");

    gasPreviouslyDetected = false;
    doorOpenedDueToGas = false;
    digitalWrite(LED_PIN, LOW);
    digitalWrite(GAS_LED_PIN, LOW);
    lockDoor();


    send_message("door/close");
    send_message("gas/alarm/off");
    if (useGasRelay) {
      isRelayOn = false;
      preferences.putBool("isRelayOn", false);
      digitalWrite(RELAY_PIN, LOW);
      send_message("relay/off");
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Password");

  } else if (message == "gas") {
    send_message(isOn ? "gas/on" : "gas/off");

  } else if (message == "gas/alarm") {
  if (isOn) {
    int gasStatus = digitalRead(GAS_PIN);
    if (gasStatus == LOW) {
      send_message("gas/alarm/on");
      if (useGasRelay) {
        isRelayOn = true;
        preferences.putBool("isRelayOn", true);
        digitalWrite(RELAY_PIN, HIGH);
        send_message("relay/on");
      }
      // Door automation: open door if enabled
      if (useGasDoor) {
        unlockDoor();
        doorOpenedDueToGas = true;
        send_message("door/open");
      }
    } else {
      send_message("gas/alarm/off");
      if (useGasRelay) {
        isRelayOn = false;
        preferences.putBool("isRelayOn", false);
        digitalWrite(RELAY_PIN, LOW);
        send_message("relay/off");
      }
      // Door automation: close door if it was opened due to gas
      if (useGasDoor && doorOpenedDueToGas) {
        lockDoor();
        doorOpenedDueToGas = false;
        send_message("door/close");
      }
    }
  } else {
    send_message("gas/alarm/off");
    if (useGasRelay) {
      isRelayOn = false;
      preferences.putBool("isRelayOn", false);
      digitalWrite(RELAY_PIN, LOW);
      send_message("relay/off");
    }
    // Door automation: close door if it was opened due to gas
    if (useGasDoor && doorOpenedDueToGas) {
      lockDoor();
      doorOpenedDueToGas = false;
      send_message("door/close");
    }
  }
} else if (message == "dht") {
    readDHT();

  } else if (message == "relay") {
    send_message(isRelayOn ? "relay/on" : "relay/off");

  } else if (message == "relay/on") {
    isRelayOn = true;
    digitalWrite(RELAY_PIN, HIGH);
    preferences.putBool("isRelayOn", true);
    send_message("relay/on");

  } else if (message == "relay/off") {
    isRelayOn = false;
    digitalWrite(RELAY_PIN, LOW);
    preferences.putBool("isRelayOn", false);
    send_message("relay/off");

  } else if (message == "gas/relay") {
  send_message(useGasRelay ? "gas/relay/on" : "gas/relay/off");

} else if (message == "gas/relay/on") {
  useGasRelay = true;
  preferences.putBool("useGasRelay", true);
  send_message("gas/relay/on");

  int gasStatus = digitalRead(GAS_PIN);
  if (gasStatus == LOW && isOn) {
    isRelayOn = true;
    digitalWrite(RELAY_PIN, HIGH);
    preferences.putBool("isRelayOn", true);
    send_message("relay/on");
  }

} else if (message == "gas/relay/off") {
  useGasRelay = false;
  preferences.putBool("useGasRelay", false);
  send_message("gas/relay/off");

  int gasStatus = digitalRead(GAS_PIN);
  if (gasStatus == LOW && isOn && isRelayOn) {
    isRelayOn = false;
    digitalWrite(RELAY_PIN, LOW);
    preferences.putBool("isRelayOn", false);
    send_message("relay/off");
  }

} else if (message == "gas/door") {
  send_message(useGasDoor ? "gas/door/on" : "gas/door/off");

} else if (message == "gas/door/on") {
  useGasDoor = true;
  preferences.putBool("useGasDoor", true);
  send_message("gas/door/on");

  int gasStatus = digitalRead(GAS_PIN);
  if (gasStatus == LOW && isOn && !isUnlocked) {
    unlockDoor();
     digitalWrite(LED_PIN, HIGH);
    doorOpenedDueToGas = true;
    send_message("door/open");
  }

} else if (message == "gas/door/off") {
  useGasDoor = false;
  preferences.putBool("useGasDoor", false);
  send_message("gas/door/off");

  int gasStatus = digitalRead(GAS_PIN);
  if (gasStatus == LOW && isOn && doorOpenedDueToGas && isUnlocked) {
    lockDoor();
         digitalWrite(LED_PIN, LOW);
    doorOpenedDueToGas = false;
     send_message("door/close");
  }
}
}

void send_message(const String& message) {
  if (client.publish(mqtt_response_topic, message.c_str())) {
    Serial.println("Message sent: " + message);
  } else {
    Serial.println("Failed to send message");
  }
}
