/************************************************************
 *                SMART HOME PROJECT (ESP32)
 *
 * Features Added Step-by-Step:
 * ------------------------------------------
 * Version 1: Basic Relay Control
 * Version 2: Bluetooth Control Added
 * Version 3: IR Remote Control Added
 * Version 4: OLED Display Integration Added
 *
 * Final System:
 * ------------------------------------------
 * - Control relays using Bluetooth 📱
 * - Control relays using IR Remote 📡
 * - Display system status on OLED 📺
 * - Show only ACTIVE (ON) relays on display
 *
 * Libraries Used:
 * ------------------------------------------
 * - Wire.h                 (I2C Communication)
 * - Adafruit_GFX.h        (Graphics library)
 * - Adafruit_SSD1306.h    (OLED driver)
 * - BluetoothSerial.h     (ESP32 Bluetooth)
 * - IRremote.hpp          (IR decoding)
 *
 * Pin Configuration:
 * ------------------------------------------
 * Relay1 → GPIO 26
 * Relay2 → GPIO 25
 * Relay3 → GPIO 33
 * Relay4 → GPIO 32
 *
 * IR Receiver → GPIO 14
 *
 * OLED (I2C):
 * SDA → GPIO 21
 * SCL → GPIO 22
 *
 ************************************************************/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "BluetoothSerial.h"
#include <IRremote.hpp>

// ---------------- OLED CONFIG ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------------- BLUETOOTH ----------------
BluetoothSerial SerialBT;

// ---------------- RELAY PINS ----------------
#define RELAY1 26
#define RELAY2 25
#define RELAY3 33
#define RELAY4 32

// ---------------- IR PIN ----------------
#define IR_PIN 14

// ---------------- RELAY STATES ----------------
// 0 = OFF, 1 = ON
bool r1 = 0, r2 = 0, r3 = 0, r4 = 0;

// Track control source (Bluetooth / IR)
String controlSource = "NONE";

// ---------------- IR FILTER ----------------
unsigned long lastIRTime = 0;
int lastIRCode = -1;
const int debounceDelay = 300;

// ---------------- SETUP ----------------
void setup() {

  Serial.begin(115200);

  Serial.println("\n==============================");
  Serial.println(" SMART HOME PROJECT (ESP32)");
  Serial.println("==============================");

  Serial.println("\nFeatures:");
  Serial.println("V1: Relay Control");
  Serial.println("V2: Bluetooth Control");
  Serial.println("V3: IR Remote Control");
  Serial.println("V4: OLED Display");

  Serial.println("\nFinal System:");
  Serial.println("- Bluetooth Control");
  Serial.println("- IR Remote Control");
  Serial.println("- OLED Status Display");

  Serial.println("\nPins:");
  Serial.println("Relay1 -> GPIO 26");
  Serial.println("Relay2 -> GPIO 25");
  Serial.println("Relay3 -> GPIO 33");
  Serial.println("Relay4 -> GPIO 32");
  Serial.println("IR -> GPIO 14");
  Serial.println("OLED SDA -> GPIO 21");
  Serial.println("OLED SCL -> GPIO 22");

  Serial.println("\nSystem Boot Complete");
  Serial.println("==============================\n");

  // Start Bluetooth
  SerialBT.begin("ESP32_SmartHome");

  // Start I2C
  Wire.begin(21, 22);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not found");
    while (1)
      ;
  }

  // Configure relay pins
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  // Initialize IR receiver
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  // Set initial relay states
  updateRelays();

  // Show initial display
  updateDisplay();

  Serial.println("Smart Home System Ready");
}

// ---------------- MAIN LOOP ----------------
void loop() {

  // ----------- BLUETOOTH CONTROL -----------
  if (SerialBT.available()) {

    char incoming = SerialBT.read();
    Serial.println(incoming);

    controlSource = "Bluetooth";

    handleBluetooth(incoming);  // Process command
    updateRelays();             // Apply changes
    updateDisplay();            // Update OLED
  }

  // ----------- IR CONTROL -----------
  if (IrReceiver.decode()) {

    // Ignore repeat signals
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
      IrReceiver.resume();
      return;
    }

    int code = IrReceiver.decodedIRData.command;
    unsigned long currentTime = millis();

    // Ignore invalid codes
    if (code == 0) {
      IrReceiver.resume();
      return;
    }

    // Accept only defined buttons
    if (code != 12 && code != 24 && code != 94 && code != 8 && code != 28 && code != 73) {
      IrReceiver.resume();
      return;
    }

    // Debounce logic
    if (code != lastIRCode || (currentTime - lastIRTime) > debounceDelay) {

      Serial.print("IR Code: ");
      Serial.println(code);

      controlSource = "IR Remote";

      handleIR(code);
      updateRelays();
      updateDisplay();

      lastIRCode = code;
      lastIRTime = currentTime;
    }

    IrReceiver.resume();
  }
}

// ---------------- APPLY RELAY STATES ----------------
void updateRelays() {

  // Active LOW relays
  digitalWrite(RELAY1, r1 ? LOW : HIGH);
  digitalWrite(RELAY2, r2 ? LOW : HIGH);
  digitalWrite(RELAY3, r3 ? LOW : HIGH);
  digitalWrite(RELAY4, r4 ? LOW : HIGH);
}

// ---------------- OLED DISPLAY ----------------
void updateDisplay() {

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Title
  display.setCursor(0, 0);
  display.println("Smart Home");

  // Source info
  display.setCursor(0, 10);
  display.print("Source: ");
  display.println(controlSource);

  // Show ONLY active relays
  display.setCursor(0, 25);
  display.print("Active: ");

  bool anyOn = false;

  if (r1) {
    display.print("R1 ");
    anyOn = true;
  }
  if (r2) {
    display.print("R2 ");
    anyOn = true;
  }
  if (r3) {
    display.print("R3 ");
    anyOn = true;
  }
  if (r4) {
    display.print("R4 ");
    anyOn = true;
  }

  // If none ON
  if (!anyOn) {
    display.print("None");
  }

  display.display();
}

// ---------------- BLUETOOTH HANDLER ----------------
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

// ---------------- IR HANDLER ----------------
void handleIR(int code) {

  switch (code) {

    case 12: r1 = !r1; break;
    case 24: r2 = !r2; break;
    case 94: r3 = !r3; break;
    case 8: r4 = !r4; break;

    case 28: r1 = r2 = r3 = r4 = 1; break;
    case 73: r1 = r2 = r3 = r4 = 0; break;
  }
}