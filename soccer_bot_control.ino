/*
 * ESP32 Bluetooth Soccer Robot - Firmware v1.1.0
 *
 * Board        : ESP32-WROOM (30-pin)
 * Motor Drivers: 2x BTS7960
 * Motors       : 2x 37GB 800 RPM metal gear motors
 * Control      : Bluetooth Serial + USB Serial
 * PWM          : 20 kHz, 8-bit
 *
 * Author: Farhan Shahriar Shihab
 */

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <esp_arduino_version.h>

BluetoothSerial SerialBT;

/* ---------- MOTOR PINS ---------- */
// Left driver
const int L_RPWM = 21;   // forward
const int L_LPWM = 19;   // backward

// Right driver
const int R_RPWM = 23;   // forward
const int R_LPWM = 22;   // backward

/* ---------- PWM SETTINGS ---------- */
const int PWM_FREQ = 20000;   // 20 kHz (no audible motor whine)
const int PWM_RES  = 8;       // 8-bit resolution (0-255)

#if ESP_ARDUINO_VERSION_MAJOR >= 3

// ESP32 Arduino core 3.x: attach by pin
bool pwmInit() {
  const int pins[4] = { L_RPWM, L_LPWM, R_RPWM, R_LPWM };
  bool success = true;

  for (int i = 0; i < 4; i++) {
    if (ledcAttach(pins[i], PWM_FREQ, PWM_RES)) {
      ledcWrite(pins[i], 0);
    } else {
      success = false;
    }
  }
  return success;
}

void pwmWrite(int pin, int duty) {
  ledcWrite(pin, constrain(duty, 0, 255));
}

#else

// ESP32 Arduino core 2.x: use LEDC channels
const int pwmPins[4] = { L_RPWM, L_LPWM, R_RPWM, R_LPWM };

bool pwmInit() {
  for (int i = 0; i < 4; i++) {
    ledcSetup(i, PWM_FREQ, PWM_RES);
    ledcAttachPin(pwmPins[i], i);
    ledcWrite(i, 0);
  }
  return true;
}

void pwmWrite(int pin, int duty) {
  for (int i = 0; i < 4; i++) {
    if (pwmPins[i] == pin) {
      ledcWrite(i, constrain(duty, 0, 255));
      return;
    }
  }
}

#endif

/* ---------- TUNING ---------- */
const int ACCEL_STEP = 28;                    // PWM added per ramp step when speeding up
const int DECEL_STEP = 40;                    // PWM removed per ramp step when slowing down
const unsigned long RAMP_MS = 4;              // time between ramp steps
const unsigned long REVERSAL_DELAY_MS = 30;   // pause at zero when reversing direction
const int  BRAKE_BELOW = 30;                  // brake when |speed| drops below this
const bool USE_BRAKE   = false;               // true = active brake when stopping

/* ---------- STATE ---------- */
int  Speed = 180;           // default speed (app slider 5)
char lastCmd = 'S';         // last movement command received
bool wasConnected = false;  // previous Bluetooth connection state
bool pwmReady = false;      // true once PWM is set up

int targetL = 0, targetR = 0;   // requested speed per side
int curL = 0,    curR = 0;      // speed currently applied per side

unsigned long holdL = 0, holdR = 0;   // reversal hold timers
unsigned long lastRamp = 0;

/* ---------- MOTOR OUTPUT ---------- */
// value: -255..255 (positive = forward, negative = backward)
void setMotor(int fwdPin, int bwdPin, int value, bool brake) {
  value = constrain(value, -255, 255);

  if (value > 0) {
    pwmWrite(bwdPin, 0);
    pwmWrite(fwdPin, value);
  } else if (value < 0) {
    pwmWrite(fwdPin, 0);
    pwmWrite(bwdPin, -value);
  } else if (brake) {
    pwmWrite(fwdPin, 255);
    pwmWrite(bwdPin, 255);
  } else {
    pwmWrite(fwdPin, 0);
    pwmWrite(bwdPin, 0);
  }
}

/* ---------- EMERGENCY STOP ---------- */
void emergencyStop() {
  lastCmd = 'S';
  targetL = targetR = 0;
  curL = curR = 0;
  holdL = holdR = 0;

  if (pwmReady) {
    setMotor(L_RPWM, L_LPWM, 0, USE_BRAKE);
    setMotor(R_RPWM, R_LPWM, 0, USE_BRAKE);
  }
}

/* ---------- RAMP CONTROL ---------- */
int stepToward(int cur, int tgt) {
  if (cur == tgt) return cur;

  bool speedingUp = (abs(tgt) > abs(cur)) && (cur == 0 || ((cur > 0) == (tgt > 0)));
  int step = speedingUp ? ACCEL_STEP : DECEL_STEP;

  if (tgt > cur) return min(cur + step, tgt);
  return max(cur - step, tgt);
}

int stepSide(int cur, int tgt, unsigned long &hold, unsigned long now) {
  if (now < hold) return 0;

  // Direction change: slow down to zero first, pause, then go the other way
  bool opposite = cur != 0 && tgt != 0 && ((cur > 0) != (tgt > 0));
  if (opposite) tgt = 0;

  int next = stepToward(cur, tgt);
  if (opposite && next == 0) hold = now + REVERSAL_DELAY_MS;

  return next;
}

/* ---------- UPDATE MOTORS (non-blocking) ---------- */
void updateMotors() {
  if (!pwmReady) return;

  unsigned long now = millis();
  if (now - lastRamp < RAMP_MS) return;
  lastRamp = now;

  curL = stepSide(curL, targetL, holdL, now);
  curR = stepSide(curR, targetR, holdR, now);

  bool brakeL = USE_BRAKE && targetL == 0 && abs(curL) < BRAKE_BELOW;
  bool brakeR = USE_BRAKE && targetR == 0 && abs(curR) < BRAKE_BELOW;

  if (brakeL) curL = 0;
  if (brakeR) curR = 0;

  setMotor(L_RPWM, L_LPWM, curL, brakeL);
  setMotor(R_RPWM, R_LPWM, curR, brakeR);
}

/* ---------- MOVEMENT COMMANDS ---------- */
void setTarget(int l, int r) {
  targetL = constrain(l, -255, 255);
  targetR = constrain(r, -255, 255);
}

void applyMove(char c) {
  switch (c) {
    case 'F': setTarget( Speed,      Speed);      break;   // forward
    case 'B': setTarget(-Speed,     -Speed);      break;   // backward
    case 'L': setTarget(-Speed,      Speed);      break;   // spin left
    case 'R': setTarget( Speed,     -Speed);      break;   // spin right
    case 'G': setTarget( Speed / 2,  Speed);      break;   // forward left
    case 'H': setTarget( Speed,      Speed / 2);  break;   // forward right
    case 'I': setTarget(-Speed / 2, -Speed);      break;   // backward left
    case 'J': setTarget(-Speed,     -Speed / 2);  break;   // backward right
    case 'S': setTarget(0, 0);                    break;   // stop
  }
}

/* ---------- SPEED CONTROL ---------- */
void setSpeed(int s) {
  Speed = constrain(s, 0, 255);
  applyMove(lastCmd);   // apply the new speed to the current movement right away
}

/* ---------- COMMAND HANDLER ---------- */
void handleCommand(char c) {
  switch (c) {
    // Speed commands (app slider 0-9)
    case '0': setSpeed(0);   break;
    case '1': setSpeed(100); break;
    case '2': setSpeed(120); break;
    case '3': setSpeed(140); break;
    case '4': setSpeed(160); break;
    case '5': setSpeed(180); break;
    case '6': setSpeed(200); break;
    case '7': setSpeed(220); break;
    case '8': setSpeed(240); break;
    case '9':               // '9' and 'q' both set full speed
    case 'q': setSpeed(255); break;

    // Movement commands
    case 'F': case 'B': case 'L': case 'R':
    case 'G': case 'H': case 'I': case 'J':
    case 'S':
      lastCmd = c;          // remember it so speed changes apply instantly
      applyMove(c);
      Serial.println(c);    // print movement commands only
      break;

    default: break;
  }
}

/* ---------- SETUP ---------- */
void setup() {
  Serial.begin(115200);
  delay(500);

  pwmReady = pwmInit();
  if (!pwmReady) return;   // do not start if PWM setup failed

  emergencyStop();
  SerialBT.begin("SoccerBot");   // Bluetooth device name
}

/* ---------- MAIN LOOP ---------- */
void loop() {
  if (!pwmReady) return;

  // Failsafe: stop the robot if Bluetooth disconnects
  bool connected = SerialBT.hasClient();
  if (wasConnected && !connected) emergencyStop();
  wasConnected = connected;

  // Bluetooth commands
  while (SerialBT.available() > 0) handleCommand(SerialBT.read());

  // USB Serial Monitor commands (for bench testing)
  while (Serial.available() > 0) handleCommand(Serial.read());

  updateMotors();
}
