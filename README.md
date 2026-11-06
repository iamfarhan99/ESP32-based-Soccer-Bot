<div align="center">

# 🏎️ ESP32 Bluetooth Soccer Robot

**High-torque, Bluetooth-controlled robot soccer platform**
built on ESP32 · dual BTS7960 drivers · 3S LiPo



![ESP32](https://img.shields.io/badge/MCU-ESP32-blue?logo=espressif&logoColor=white)




![Arduino](https://img.shields.io/badge/Firmware-Arduino%20C%2B%2B-00979D?logo=arduino&logoColor=white)




![Bluetooth](https://img.shields.io/badge/Control-Bluetooth%20Serial-0082FC?logo=bluetooth&logoColor=white)




![Drivers](https://img.shields.io/badge/Drivers-2x%20BTS7960-orange)




![Battery](https://img.shields.io/badge/Power-11.1V%203S%20LiPo-yellow)




![Status](https://img.shields.io/badge/Status-Competition%20Tested-brightgreen)





![Soccer Bot Action](assets/media/bot_demo.gif)



</div>

---

## 📖 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Hardware](#️-hardware)
- [Wiring & Pin Mapping](#-wiring--pin-mapping)
- [Firmware](#-firmware)
- [Quick Start](#-quick-start)
- [Command Reference](#-command-reference)
- [Competitions](#-competitions)
- [Roadmap](#️-roadmap)
- [Gallery](#-gallery)
- [Author](#-author)

---

## 📌 Overview

This project is a heavy-duty soccer robot designed for speed, pushing power and quick maneuvering. An **ESP32** receives single-character commands over **Bluetooth Serial** from a phone and converts them into 20 kHz PWM signals for two **BTS7960** H-bridge motor drivers. An **11.1V 3S LiPo** powers the motors directly, while an **XL4015** buck converter provides 5V for the ESP32.

---

## ✨ Features

- ⚡ **High acceleration:** battery voltage goes straight to the BTS7960 drivers for maximum torque.
- 🎮 **Phone control:** drive over Bluetooth with 10 speed levels (0-9).
- 🔄 **Differential steering:** spin turns and gentle arc turns in all four diagonal directions.
- 🧈 **Smooth acceleration ramp:** direction changes are stepped, so there is no instant full-current reversal on the drivers and battery.
- 🛡️ **Disconnect failsafe:** the robot stops automatically if Bluetooth disconnects.
- 🔇 **20 kHz PWM:** no audible motor whine.
- 🧩 **Simple firmware:** one readable file, easy to tune.

---

## 🛠️ Hardware

| Component | Specification | Purpose |
|:--|:--|:--|
| **Microcontroller** | ESP32 (classic, WROOM-32) | Bluetooth receiver and PWM generation |
| **Motor drivers** | 2x BTS7960 (43A peak) | High-current motor control, one per side |
| **Motors** | 2x 37GB 800 RPM metal gear motors | Driving speed and pushing torque |
| **Battery** | 11.1V 3S LiPo (high C-rating) | Main power supply |
| **Regulator** | XL4015 step-down module | 11.1V → 5V for ESP32 VIN |

> ⚠️ Set the XL4015 output to **5V with a multimeter before connecting the ESP32.**

### Power flow

```
                 [ 11.1V 3S LiPo ]
                        │
          ┌─────────────┴─────────────┐
          ▼                           ▼
 [ BTS7960 Driver L & R ]     [ XL4015 Buck Converter ]
  (high-current line)           (11.1V → 5V)
          │                           │
          ▼                           ▼
   [ 2x 37GB Motors ]          [ ESP32 VIN & GND ]
                                      │
                                      ▼
                           [ Bluetooth Serial: "SoccerBot" ]
                          (PWM → BTS7960 RPWM / LPWM inputs)
```



![Circuit Diagram](assets/hardware/Circuit_diagram.png)



---

## 🔌 Wiring & Pin Mapping

| Signal | ESP32 GPIO | Function |
|:--|:--:|:--|
| Left driver `RPWM` | **21** | Left side forward |
| Left driver `LPWM` | **19** | Left side backward |
| Right driver `RPWM` | **23** | Right side forward |
| Right driver `LPWM` | **22** | Right side backward |

- `VCC`, `R_EN` and `L_EN` of both BTS7960 modules are tied to the ESP32 **3V3** pin, so no enable pins are used in code.
- **All grounds must be common:** battery, drivers, buck converter and ESP32.

---

## 💻 Firmware

File: [`soccer_bot_control.ino`](soccer_bot_control.ino)

| Item | Detail |
|:--|:--|
| Board | **ESP32 classic** (DevKit / WROOM-32). `BluetoothSerial` does not work on ESP32-S3 / C3. |
| Core | Arduino-ESP32 **2.x and 3.x** both supported (auto-detected at compile time) |
| Bluetooth name | `SoccerBot` |
| PWM | 20 kHz, 8-bit (0-255) |
| Failsafe | Stops immediately on Bluetooth disconnect |
| Acceleration | Ramped (tunable with `RAMP_STEP` and `RAMP_INTERVAL_MS`) |

---

## 🚀 Quick Start

1. Install the **ESP32 board package** in Arduino IDE.
2. Open `soccer_bot_control.ino` (the IDE will offer to place it in a folder of the same name, click **OK**).
3. Select your ESP32 board and port, then **Upload**.
4. Power the robot, then pair your phone with **`SoccerBot`**.
5. Connect with a Bluetooth RC-car style controller app that sends the characters listed below.

> 🧪 First test with the wheels **off the ground**.

---

## 🎮 Command Reference

| Character | Action |
|:--:|:--|
| `F` / `B` | Forward / Backward |
| `L` / `R` | Spin left / Spin right |
| `G` / `H` | Forward-left / Forward-right |
| `I` / `J` | Backward-left / Backward-right |
| `S` | Stop |
| `0` – `9`, `q` | Speed level (`0` = stop, `5` = default 180, `9` / `q` = full 255) |

---

## 🏆 Competitions

### ⚽ ROBO LEAGUE 2026: BRAC University (ROBU)

**Date:** November 2026 · **Role:** Hardware debugging lead

The bot accelerated and maneuvered well early in the match. After a hard collision, the Bluetooth link became unstable and dropped, and the ESP32 felt unusually hot, so we withdrew from the match for safety. The suspected causes (supply droop, loose wiring, motor-induced noise) are under investigation.

👉 [**Read the full technical report and gallery ➔**](competitions/ROBO-LEAGUE-2026/README.md)

---

## 🗺️ Roadmap

- [ ] Solder everything on a dot-board / custom PCB instead of jumper wires
- [ ] Bulk capacitor on the 5V logic rail
- [ ] 0.1 µF ceramic capacitors across the motor terminals
- [ ] Star-ground layout and a battery fuse
- [ ] Shock-absorbing mounting for the ESP32 and strain relief for wires
- [ ] Sustained stall and collision stress testing

---

## 📸 Gallery

<div align="center">



![Build Progress](assets/hardware/soccer-bot.jpg)





![Soccer Bot Hardware Showcase](assets/media/bot_showcase.gif)



**Core Team**



![Team Work](assets/media/Team%20Work.jpg)



</div>

---

## 👤 Author

Designed, built and programmed by **Farhan** ([@iamfarhan99](https://github.com/iamfarhan99)) and the core team at **BRAC University Robotics Club (ROBU)**.
