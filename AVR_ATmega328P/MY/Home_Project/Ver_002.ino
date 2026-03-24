#include "BluetoothSerial.h"
#include <IRremote.hpp>

BluetoothSerial SerialBT;

#define RELAY1 26
#define RELAY2 25
#define RELAY3 33
#define RELAY4 32

#define IR_PIN 14

char incoming;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Relay_Control");

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  digitalWrite(RELAY1, HIGH);
  digitalWrite(RELAY2, HIGH);
  digitalWrite(RELAY3, HIGH);
  digitalWrite(RELAY4, HIGH);

  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  Serial.println("Bluetooth + IR Ready");
}

void loop() {

  // 🔵 BLUETOOTH CONTROL
  if (SerialBT.available()) {
    incoming = SerialBT.read();
    Serial.println(incoming);

    controlRelay(incoming);
  }

  // 📡 IR REMOTE CONTROL
  if (IrReceiver.decode()) {
    uint32_t code = IrReceiver.decodedIRData.command;
    Serial.print("IR Code: ");
    Serial.println(code);

    // Map your remote buttons
    switch(code) {
      case 0x45: controlRelay('1'); break; // Relay1 ON
      case 0x46: controlRelay('2'); break;

      case 0x47: controlRelay('3'); break;
      case 0x44: controlRelay('4'); break;

      case 0x40: controlRelay('5'); break;
      case 0x43: controlRelay('6'); break;

      case 0x07: controlRelay('7'); break;
      case 0x15: controlRelay('8'); break;

      case 0x09: controlRelay('A'); break; // ALL ON
      case 0x16: controlRelay('B'); break; // ALL OFF
    }

    IrReceiver.resume();
  }
}

// 🔧 Common control function
void controlRelay(char cmd) {
  switch(cmd) {
    case '1': digitalWrite(RELAY1, LOW); break;
    case '2': digitalWrite(RELAY1, HIGH); break;

    case '3': digitalWrite(RELAY2, LOW); break;
    case '4': digitalWrite(RELAY2, HIGH); break;

    case '5': digitalWrite(RELAY3, LOW); break;
    case '6': digitalWrite(RELAY3, HIGH); break;

    case '7': digitalWrite(RELAY4, LOW); break;
    case '8': digitalWrite(RELAY4, HIGH); break;

    case 'A':
      digitalWrite(RELAY1, LOW);
      digitalWrite(RELAY2, LOW);
      digitalWrite(RELAY3, LOW);
      digitalWrite(RELAY4, LOW);
      break;

    case 'B':
      digitalWrite(RELAY1, HIGH);
      digitalWrite(RELAY2, HIGH);
      digitalWrite(RELAY3, HIGH);
      digitalWrite(RELAY4, HIGH);
      break;
  }
}