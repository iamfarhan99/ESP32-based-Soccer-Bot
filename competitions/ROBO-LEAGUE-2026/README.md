# ROBO LEAGUE 2026

This is our report for ROBO LEAGUE 2026, organized by the Robotics Club of BRAC University (ROBU). It covers our robot, what happened in the match, and what we plan to fix.

## Event Details

- **Event:** ROBO LEAGUE 2026
- **Organizer:** Robotics Club of BRAC University (ROBU)
- **Category:** Soccer Bot
- **Date:** November 2026
- **Control:** Bluetooth
- **Team:** Farhan, Shuvo, Rahib, Sajid, Joya

## Robot Setup

- ESP32 for Bluetooth and motor control
- Two BTS7960 motor drivers
- Two 37GB 800 RPM metal gear motors
- 11.1V 3S LiPo battery
- XL4015 buck converter (11.1V to 5V for the ESP32)

For the wiring and pin details, see the [main README](../../README.md).

## Match Summary

At the start of the match, the robot responded well to commands. It accelerated fast and turned easily.

After a hard collision, the Bluetooth connection became unstable and then dropped completely. The robot stopped responding. When we checked it, the ESP32 was unusually hot. We left the match to protect the hardware.

## What Went Wrong

What we saw:

1. The Bluetooth connection became unstable after a collision and then dropped.
2. The robot stopped responding to commands.
3. The ESP32 was very hot afterwards.

We have not found the exact cause yet. These are the most likely reasons:

1. **Voltage drop on the 5V line.** In a collision, the motors can pull a large current. This can pull down the battery voltage and the 5V output. The ESP32 can lose Bluetooth when its supply drops. To check this, measure the 5V line while the motors are stalled.
2. **Loose wires.** Jumper wires can move or touch each other during a hit. To check this, inspect every connection and test again after soldering.
3. **Electrical noise and ground problems.** Large motor currents can disturb the ESP32 through shared ground wires. To check this, use a star ground and add small capacitors across the motor terminals.
4. **Heat in the ESP32 board regulator.** The small regulator on the ESP32 board can get hot if the supply or the load is not stable. To check this, measure the 5V line and the board temperature under load.

The BTS7960 already has built-in diodes for motor voltage spikes, so extra flyback diodes are not our first priority.

Testing is still going on to find the exact fault.

## Planned Fixes

Hardware:
- Solder everything on a dot board or PCB.
- Add a large capacitor (470 to 1000 µF) on the 5V line.
- Add a 0.1 µF ceramic capacitor across each motor's terminals.
- Use a star ground and add a battery fuse.
- Mount the ESP32 with shock protection and add strain relief on the wires.

Firmware:
- Speed ramping, so the motors do not reverse at full power in one instant. This is already in the current code.
- An optional timeout that stops the robot if no command arrives.

Testing:
- Run stall tests and repeated collision tests before the next event.

## Certificate

The participation certificate will be added after ROBU issues it.

<!-- When the certificate is ready, upload it to competitions/Assets/robu_2026_certificate.jpg and remove the arrows around the next line.


![Participation Certificate](https://github.com/iamfarhan99/ESP32-based-Soccer-Bot/raw/main/competitions/Assets/robu_2026_certificate.jpg)


-->

## Photos

**Robot lineup**



![Robot Lineup](https://github.com/iamfarhan99/ESP32-based-Soccer-Bot/raw/main/competitions/ROBO-LEAGUE-2026/Assets/Lineup.jpg)



**Competition arena**



![Competition Arena](https://github.com/iamfarhan99/ESP32-based-Soccer-Bot/raw/main/competitions/ROBO-LEAGUE-2026/Assets/COMPETITION_ARENA.jpg)



**During the match**



![Match](https://github.com/iamfarhan99/ESP32-based-Soccer-Bot/raw/main/competitions/ROBO-LEAGUE-2026/Assets/ROBOT_ARENA.jpg)



## What We Learned

- A robot must survive collisions. Good wiring, stable power and strong mounting matter as much as the code.
- Wireless problems can come from the power supply, not only from the radio.
- Writing down what failed helps us build a better version next time.
