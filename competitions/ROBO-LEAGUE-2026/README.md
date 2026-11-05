# 🏟️ ROBO LEAGUE 2026 – BRACU ROBU Soccer Bot Competition

A comprehensive competition documentation and technical post-mortem of our ESP32-based Bluetooth soccer robot, covering hardware setup, match performance, failure analysis, and future engineering iterations.

---

## 📌 1. Event Overview

| Information | Details |
| :--- | :--- |
| **Event** | ROBO LEAGUE 2026 |
| **Organizer** | Robotics Club of BRAC University (ROBU) |
| **Category** | Soccer Bot Competition |
| **Robot Platform** | ESP32 Microcontroller |
| **Control Method** | Bluetooth Wireless Control |
| **Motor Drivers** | Dual BTS7960 High-Current Drivers |
| **Power Source** | 11.1V 3S LiPo Battery |
| **Status** | Participated & Field Tested |

---

## 🤖 2. Robot Architecture

The robot was designed around an ESP32 microcontroller, providing high-speed wireless Bluetooth communication and real-time PWM motor control.

### Key Hardware Components
* **ESP32:** Core microcontroller handling Bluetooth receiver logic and PWM motor control.
* **BTS7960 Motor Drivers:** High-current H-bridge drivers for rapid direction switching and high torque.
* **DC Gear Motors:** High-RPM motors responsible for robot navigation and ball interaction.
* **11.1V 3S LiPo Battery:** Primary high-discharge power source.
* **XL4015 Buck Converter:** Step-down converter regulating battery voltage to a stable 5V supply for the ESP32 logic rail.

---

## ⚽ 3. Competition Experience

Participating in ROBO LEAGUE 2026 provided practical experience operating a self-built soccer robot in a live match environment.

* During the initial phase, the robot responded well to Bluetooth commands, demonstrating good acceleration and maneuverability.
* Following a strong physical collision mid-game, the Bluetooth connection became unstable and eventually disconnected completely. 
* Upon physical inspection, the ESP32 chip felt unusually hot.
* Due to total communication loss, we stepped back from the match for safety and hardware analysis.

---

## 🔍 4. Failure Analysis & Post-Mortem

### Observed Issues
1. **Bluetooth Disconnection:** Total loss of wireless telemetry following physical impact.
2. **ESP32 Overheating:** Microcontroller experienced sudden thermal runaway / power surge.
3. **Control Lock:** Complete loss of command execution.

### Potential Causes Under Investigation
* **Wiring Displacement:** Loose jumper wires shifting during impact, causing a momentary short circuit on logic pins.
* **Motor Back-EMF:** High inductive spikes generated during motor stall/collision feeding back into the logic circuit without optocoupler isolation.
* **Regulator Stress:** Over-current entering the XL4015 buck converter or ESP32 internal regulator.

> **Note:** Hardware inspection and diagnostic testing are currently ongoing to isolate the exact fault component.

---

## 🛠️ 5. Future Improvement Roadmap

The following design upgrades are planned for the next hardware revision:

-  Replace loose jumper wires with a soldered dot-board or custom PCB layout.
-  Add optocoupler isolation between ESP32 GPIOs and motor driver logic pins.
-  Improve shock-absorption padding around the microcontroller chassis.
-  Implement flyback protection against motor inductive current spikes.
-  Perform stress testing under sustained high-load stall conditions.

---

## 📜 6. Certification & Gallery

### 📄 Participation Certificate
*(Official certificate will be uploaded once issued by ROBU)*
![Official Certificate](../assets/robu_2026_certificate.jpg)

---

### 📸 Competition Gallery

#### Robot Lineup
![Robot Lineup](../assets/Lineup.jpg)

#### Competition Arena
![Competition Arena](../assets/COMPETITION_ARENA.jpg)

#### Match Experience
![Match Experience](../assets/ROBOT_ARENA.jpg)

---

## 📚 7. Key Takeaways

This competition provided valuable hands-on experience in:
* Real-world embedded systems operation under competitive stress.
* Wireless telemetry debugging in noisy environments.
* Identifying physical collision vulnerability in prototype robotics hardware.
* Performing structured failure analysis to improve future iterations.
