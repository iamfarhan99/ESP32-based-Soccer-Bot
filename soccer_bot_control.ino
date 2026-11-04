/*
 * ESP32 High-Speed Soccer Robot Firmware
 * Microcontroller: ESP32
 * Motor Drivers: Dual BTS7960
 * Author: Farhan (BRACU ROBU)
 */

#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

/* ---------- PIN SETUP (2x BTS7960, as per wiring diagram) ---------- */
// LEFT driver (orange wires)
const int L_RPWM = 21;   // forward
const int L_LPWM = 19;   // backward

// RIGHT driver (purple wires)
const int R_RPWM = 23;   // forward
const int R_LPWM = 22;   // backward

// VCC, R_EN and L_EN are all tied to 3V3, so no EN pins are used in code

/* ---------- PWM SETTINGS ---------- */
const int PWM_FREQ = 20000;   // 20 kHz (no audible motor whine)
const int PWM_RES  = 8;       // 8-bit resolution (0-255)

const int pwmPins[4] = { L_RPWM, L_LPWM, R_RPWM, R_LPWM };

#if ESP_ARDUINO_VERSION_MAJOR >= 3
  // ESP32 Arduino core 3.x: attach by pin
  void pwmInit() {
    for (int i = 0; i < 4; i++) ledcAttach(pwmPins[i], PWM_FREQ, PWM_RES);
  }
  void pwmWrite(int i, int duty) { ledcWrite(pwmPins[i], duty); }
#else
  // ESP32 Arduino core 2.x: use LEDC channels
  void pwmInit() {
    for (int i = 0; i < 4; i++) {
      ledcSetup(i, PWM_FREQ, PWM_RES);
      ledcAttachPin(pwmPins[i], i);
    }
  }
  void pwmWrite(int i, int duty) { ledcWrite(i, duty); }
#endif

/* ---------- STATE ---------- */
int  Speed = 180;          // default speed (app slider 5)
char lastCmd = 'S';        // last movement command received
bool wasConnected = false; // previous Bluetooth connection state

/* v: -255..255 (positive = forward, negative = backward) */
void setLeft(int v) {
  v = constrain(v, -255, 255);
  if (v >= 0) { pwmWrite(0, v);  pwmWrite(1, 0);  }
  else        { pwmWrite(0, 0);  pwmWrite(1, -v); }
}

void setRight(int v) {
  v = constrain(v, -255, 255);
  if (v >= 0) { pwmWrite(2, v);  pwmWrite(3, 0);  }
  else        { pwmWrite(2, 0);  pwmWrite(3, -v); }
}

void drive(int left, int right) {
  setLeft(left);
  setRight(right);
}

void stopAll() { drive(0, 0); }

// Executes a movement command using the current Speed
void applyMove(char c) {
  switch (c) {
    case 'F': drive( Speed,  Speed); break;          // forward
    case 'B': drive(-Speed, -Speed); break;          // backward
    case 'L': drive(-Speed,  Speed); break;          // spin left
    case 'R': drive( Speed, -Speed); break;          // spin right
    case 'G': drive( Speed / 2,  Speed); break;      // forward left
    case 'H': drive( Speed,  Speed / 2); break;      // forward right
    case 'I': drive(-Speed / 2, -Speed); break;      // backward left
    case 'J': drive(-Speed, -Speed / 2); break;      // backward right
    case 'S': stopAll(); break;                      // stop
  }
}

void setup() {
  Serial.begin(115200);

  pwmInit();
  stopAll();

  SerialBT.begin("SoccerBot");   // Bluetooth device name
  Serial.println("SoccerBot ready");
}

void loop() {
  // Failsafe: stop the robot automatically if Bluetooth disconnects
  bool connected = SerialBT.hasClient();
  if (wasConnected && !connected) { lastCmd = 'S'; stopAll(); }
  wasConnected = connected;

  while (SerialBT.available() > 0) {
    char c = SerialBT.read();
    Serial.println(c);   // debug: print received character

    switch (c) {
      // Speed commands (app slider 0-9)
      case '0': Speed = 0;   applyMove(lastCmd); break;
      case '1': Speed = 100; applyMove(lastCmd); break;
      case '2': Speed = 120; applyMove(lastCmd); break;
      case '3': Speed = 140; applyMove(lastCmd); break;
      case '4': Speed = 160; applyMove(lastCmd); break;
      case '5': Speed = 180; applyMove(lastCmd); break;
      case '6': Speed = 200; applyMove(lastCmd); break;
      case '7': Speed = 220; applyMove(lastCmd); break;
      case '8': Speed = 240; applyMove(lastCmd); break;
      case '9':               // fall-through: '9' and 'q' both set full speed
      case 'q': Speed = 255; applyMove(lastCmd); break;

      // Movement commands
      case 'F': case 'B': case 'L': case 'R':
      case 'G': case 'H': case 'I': case 'J':
      case 'S':
        lastCmd = c;         // remember the command so speed changes apply instantly
        applyMove(c);
        break;

      default: break;   // horn (Y) and light (X/x) are not used
    }
  }
}
