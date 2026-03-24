#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

#define RELAY1 12
#define RELAY2 13
#define RELAY3 14
#define RELAY4 27

char incoming;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Relay_Control"); // Bluetooth name

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  // OFF initially (Active LOW)
  digitalWrite(RELAY1, HIGH);
  digitalWrite(RELAY2, HIGH);
  digitalWrite(RELAY3, HIGH);
  digitalWrite(RELAY4, HIGH);

  Serial.println("Bluetooth Started. Waiting for commands...");
}

void loop() {
  if (SerialBT.available()) {
    incoming = SerialBT.read();
    Serial.println(incoming);

    switch(incoming) {
      case '1': digitalWrite(RELAY1, LOW); break;
      case '2': digitalWrite(RELAY1, HIGH); break;

      case '3': digitalWrite(RELAY2, LOW); break;
      case '4': digitalWrite(RELAY2, HIGH); break;

      case '5': digitalWrite(RELAY3, LOW); break;
      case '6': digitalWrite(RELAY3, HIGH); break;

      case '7': digitalWrite(RELAY4, LOW); break;
      case '8': digitalWrite(RELAY4, HIGH); break;

      case 'A': // ALL ON
        digitalWrite(RELAY1, LOW);
        digitalWrite(RELAY2, LOW);
        digitalWrite(RELAY3, LOW);
        digitalWrite(RELAY4, LOW);
        break;

      case 'B': // ALL OFF
        digitalWrite(RELAY1, HIGH);
        digitalWrite(RELAY2, HIGH);
        digitalWrite(RELAY3, HIGH);
        digitalWrite(RELAY4, HIGH);
        break;
    }
  }
}