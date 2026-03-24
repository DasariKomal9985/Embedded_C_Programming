#include "BluetoothSerial.h"
#include <IRremote.hpp>

BluetoothSerial SerialBT;

// ✅ Relay pins (safe pins)
#define RELAY1 26
#define RELAY2 25
#define RELAY3 33
#define RELAY4 32

// ✅ IR pin
#define IR_PIN 14

// ✅ Relay states (0 = OFF, 1 = ON)
bool r1 = 0, r2 = 0, r3 = 0, r4 = 0;

char incoming;

// ✅ IR debounce variables
unsigned long lastIRTime = 0;
int lastIRCode = -1;
const int debounceDelay = 300;  // milliseconds

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Relay_Control");

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  // Initialize IR receiver
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  updateRelays();

  Serial.println("System Ready: Bluetooth + IR + Relay");
}

void loop() {

  // 🔵 Bluetooth Control (Toggle-based)
  if (SerialBT.available()) {
    incoming = SerialBT.read();
    Serial.print("BT: ");
    Serial.println(incoming);

    handleBluetooth(incoming);
    updateRelays();
  }

  // 📡 IR Remote Control (with debounce + repeat filter)
  if (IrReceiver.decode()) {

    // Ignore repeat signals
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
      IrReceiver.resume();
      return;
    }

    int code = IrReceiver.decodedIRData.command;
    unsigned long currentTime = millis();

    // ❌ Ignore invalid codes
    if (code == 0) {
      IrReceiver.resume();
      return;
    }

    // ✅ Allow only your valid codes
    if (code != 12 && code != 24 && code != 94 && code != 8 && code != 28 && code != 73) {
      IrReceiver.resume();
      return;
    }

    // Debounce logic
    if (code != lastIRCode || (currentTime - lastIRTime) > debounceDelay) {

      Serial.print("IR Code: ");
      Serial.println(code);

      handleIR(code);
      updateRelays();

      lastIRCode = code;
      lastIRTime = currentTime;
    }

    IrReceiver.resume();
  }
}

// 🔧 Apply relay states to hardware
void updateRelays() {
  digitalWrite(RELAY1, r1 ? LOW : HIGH);
  digitalWrite(RELAY2, r2 ? LOW : HIGH);
  digitalWrite(RELAY3, r3 ? LOW : HIGH);
  digitalWrite(RELAY4, r4 ? LOW : HIGH);

  // Debug print
  Serial.printf("R1:%d R2:%d R3:%d R4:%d\n", r1, r2, r3, r4);
}

// 🔵 Bluetooth toggle control
void handleBluetooth(char cmd) {
  switch (cmd) {
    case '1': r1 = !r1; break;
    case '2': r2 = !r2; break;
    case '3': r3 = !r3; break;
    case '4': r4 = !r4; break;

    case '5': r1 = r2 = r3 = r4 = 1; break;  // ALL ON
    case '6': r1 = r2 = r3 = r4 = 0; break;  // ALL OFF
  }
}

// 📡 IR control (your given codes)
void handleIR(int code) {
  switch (code) {
    case 12: r1 = !r1; break;
    case 24: r2 = !r2; break;
    case 94: r3 = !r3; break;
    case 8: r4 = !r4; break;

    case 28: r1 = r2 = r3 = r4 = 1; break;  // ALL ON
    case 73: r1 = r2 = r3 = r4 = 0; break;  // ALL OFF
  }
}