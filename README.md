# Nautinect Autopilot – ESP32 BLE

## License
This software is free to use, modify, and distribute **for non-commercial purposes only**.  
Commercial use (including selling, offering services, or integrating into commercial products) is strictly prohibited.
This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License. https://creativecommons.org/licenses/by-nc-sa/4.0/


---

## Installation Guide
To set up the code and motor driver, I created a short video tutorial:  
👉 https://youtu.be/zypO15Sglec  

The complete firmware (including built-in compass, NMEA, clutch, and other features) is delivered via an **OTA update** from the phone app to the ESP32.  

For a full assembly guide covering all components (including internal compass, wireless remote, and NMEA), go to:  
👉 https://nautinect.com/buildyourownautopilot  

---

## Shop – Nautinect.com
On [nautinect.com](https://nautinect.com) you can order DIY packages, including a **pre-programmed microcontroller**.  
You can also purchase the **Full Features Pilot**, which includes a wireless remote, current sensing, and an onboard compass.

---

## Facebook Group
Please post your questions in the Facebook group so others can also help, and the answers remain visible to everyone.  
Facebook is not my preferred medium, but it was the most practical option.  

👉 Join here: [Nautinect Facebook Group](https://www.facebook.com/groups/721879886302423)  

---

## Autopilot – ESP32 BLE
This code can be flashed to an ESP32 using the **Arduino SDK**.  

It can run as a **standalone autopilot based on compass steering**, or you can use it with the phone app:  
- [Android](https://play.google.com/apps/testing/com.sailtactical.autopilot)  
- [iOS](https://apps.apple.com/nl/app/nautinect-autopilot/id6473739289)  

When using the app, the **compass, gyro, and GPS of your phone** will be used.  
In that case, you only need an **ESP32** and a **motor driver**.  

- Wire the motor driver to the ESP32.  
- Connect **VCC** and **GND** as described in the documentation.  
- Connect **GPIO 33** → motor-driver-left.  
- Connect **GPIO 32** → motor-driver-right.  

---

## ESP32 Driver
If your computer does not recognize the ESP32 when you plug it in, install the **CP210x driver**:  
👉 https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads  

---

## Installation Methods

### 1. Web Flasher (EASY)  
👉 [http://nautinect.com/firmware-flasher](http://nautinect.com/firmware-flasher)  

### 2. Arduino SDK V2 (ADVANCED)
1. Download and install [Arduino IDE V2](https://www.arduino.cc/en/software)  
2. Go to **Tools → Board Manager**, search for `esp32`, and install **ESP32 by Espressif Systems**.  
3. Create a folder named `autopilot_basic`.  
   - Copy all `.ino`, `.cpp`, and `.h` files from the repository into this folder.  
   - Open `autopilot_ble.ino` with the Arduino IDE.  
4. Select your board (e.g., **DEVKIT ESP32**) and COM port.  
   - If the ESP boards do not appear, make sure you added ESP32 support to the Arduino IDE.  

Now open the app on your phone. It will search for a Bluetooth device named **autopilot**.  
When found, connect to it.  

---

## Hardware
1. **ESP32** – preferably a USB-C type with 30 pins (CH340C).  
2. **Motor driver** – IBT-2 (max 40A) or L298N (max 2A).  
   - ⚠️ Many clones exist, and behavior may differ.  
   - The +5V output is not always available.  
   - Faulty drivers are common → order at least 2.  
3. Jumper wires.  
4. **5V supply** for ESP32 (options):  
   - Powerbank  
   - USB socket  
   - +5V from L298N  
   - Voltage regulator (step down 12V/24V → 5V)  
   - Phone with USB OTG cable  
   - ESP32 expansion board (rated up to 16V)  

---

## Wiring (IBT-2 → ESP32)
```text
1. VCC   → +5V from ESP32
2. GND   → GND from ESP32
3. R_IS  → Not connected
4. L_IS  → Not connected
5. R_EN  → +5V from ESP32
6. L_EN  → +5V from ESP32
7. R_PWM → GPIO 32 on ESP32
8. L_PWM → GPIO 33 on ESP32
# ⚓ Command API Documentation

## Table of Contents
- [Overview](#overview)
- [Communication Channels](#communication-channels)
- [Command Format](#command-format)
- [Command Reference](#command-reference)
  - [Autopilot Commands](#autopilot-commands)
  - [Calibration Commands](#calibration-commands)
  - [PID & Motor Configuration](#pid--motor-configuration)
  - [Network Configuration](#network-configuration)
  - [Rudder Configuration](#rudder-configuration)
  - [System Commands](#system-commands)
- [Enumerations](#enumerations)
- [NMEA Integration](#nmea-integration)
- [Example Usage](#example-usage)
- [Parser Behavior](#parser-behavior)
- [Firmware Requirements](#firmware-requirements)
- [References](#references)

---

## Overview

All commands (from Serial, Wi-Fi, NMEA0183, or NMEA2000) are processed through a unified command handler:

```cpp
void processCommand(const std::string& rxString);
```

All messages are strings but are automatically parsed and converted to their required data types (int, float, bool, enum, etc.).

---

## API - Communication channels

| Channel | Protocol | Description |  Example |
|---------|----------|-------------|-------------|
| **Serial** | UART @ 115200 baud | Used for debug or direct control | Serial.print("3,0") |
| **NMEA0183 (wired)** | RS-422 @ 4800 baud | Receives commands with sentence $APCMD | "$APCMD,3,0" |
| **NMEA0183 (Wi-Fi)** |UDP server | Same syntax as serial commands | "$APCMD,3,0" |
| **NMEA2000** | PGN 743837 | Nautinect autopilot control PGN | sendN2K(pgn: 743837. payload: "3,0" |
| **Bluetooth LE** | GATT service | Used by the mobile app.<br>Serice UUID = ab0828b1-198e-4351-b779-901fa0e0371e <br>Characteristic UUID 4ac8a682-9736-4e5d-932b-e9b31405049c| Bluetooth::notify(commandNotifyCharacteristic, "3,0") |

Independent from the communication channel used, the autopilot expects a string containing the required parameters and will be parsed inside the firmware.
---

## Command Format

Commands follow a simple CSV-like format:

```
<CommandID>,<value>[,<value2>][,<value3>]
```

### Examples

```
3,0            → ap_on (headingSource = 0)
2,180          → ap_target (target heading = 180°)
1,200,500      → motor_set (speed = 200, duration = 500 ms)
```

---

## Command Reference

### Autopilot Commands

| ID | Command | Description | Parameters |
|----|---------|-------------|------------|
| **1** | `motor_set` | Set motor speed directly | `int speed` (−255…255), `int duration_ms` _(optional)_ |
| **2** | `ap_target` | Set target heading | `int heading_deg` (0–359) |
| **3** | `ap_on` | Enable autopilot | `int headingSource` (see [HeadingSource](#headingsource)) |
| **4** | `ap_off` | Disable autopilot | _(no parameters)_ |
| **5** | `ap_mode` | Select autopilot mode | `int mode` (0=heading, 1=route, 2=wind) |
| **6** | `heading_source` | Change heading input source | `int source` (see [HeadingSource](#headingsource)) |
| **7** | `calibrate_gyro` | Start gyroscope calibration | _(no parameters)_ |
| **8** | `mag_continuous` | Enable continuous magnetometer calibration | `"1"` = on, `"0"` = off |
| **9** | `calibrate_mag` | Start magnetometer calibration | _(no parameters)_ |
| **10** | `mag_default` | Reset magnetometer to default calibration | _(no parameters)_ |
| **11** | `get_mag_cal_backup` | Read magnetometer backup from EEPROM | _(no parameters)_ |
| **12** | `set_mag_cal_backup` | Save current calibration as backup | _(no parameters)_ |
| **13** | `set_compass_heading` | Manually adjust compass offset | `int new_heading_deg` |
| **14–20** | `gain_*`, `precision` | PID tuning parameters | `int gain` (0–255) |
| **22–30** | Motor configuration | Speed limits, current limits, motor type, etc. | _(various)_ |
| **31–34** | Wi-Fi configuration | Enable Wi-Fi, set SSID, password, port | _(various)_ |
| **35** | `get_calibration` | Print current calibration info | _(no parameters)_ |
| **36–40** | Rudder configuration | Feedback sensor, limits, rudder counts | _(various)_ |
| **41** | `set_tack_angle` | Set sailing tack angle | `int degrees` |
| **42** | `rudder_range` | Set maximum rudder angle range | `int degrees` |
| **253** | `send_settings` | Send all stored settings to app | _(no parameters)_ |
| **254** | `protocol_version` | Return current protocol version | _(no parameters)_ |
|----------|---------|-------------|------------|


## Enumerations

### ApMode

Defines the autopilot operating mode.

```cpp
enum ApMode { heading, route, wind };
```

| Value | Mode | Description |
|-------|------|-------------|
| **0** | `heading` | Heading-hold mode (maintain compass course) |
| **1** | `route` | Route-follow mode (follow NMEA waypoints) |
| **2** | `wind` | Wind-angle mode (maintain angle relative to wind) |

---

### HeadingSource

Defines the source of heading information.

```cpp
enum HeadingSource {
  autopilotCompass,
  phoneCompass,
  phoneGps,
  AWA,
  TWA,
  NMEA_GPS,
  NMEA_Compass
};
```

| Value | Source | Description |
|-------|--------|-------------|
| **0** | `autopilotCompass` | Internal compass (sensor unit) |
| **1** | `phoneCompass` | Smartphone compass via Bluetooth |
| **2** | `phoneGps` | Smartphone GPS heading via Bluetooth |
| **3** | `AWA` | Apparent Wind Angle (from wind sensor) |
| **4** | `TWA` | True Wind Angle (calculated) |
| **5** | `NMEA_GPS` | External NMEA GPS heading |
| **6** | `NMEA_Compass` | External NMEA compass heading |


## Parser Behavior

- **All received values are strings** that are parsed by the command processor
- Conversion is done internally using `std::stoi()` and `std::stof()` based on expected parameter types
- **Invalid values are ignored** and an error message is sent via Serial output
- **Valid settings are automatically saved** to EEPROM for persistence across reboots
- Commands are case-sensitive and must match the expected format exactly

### Error Handling

When an invalid command is received:
1. An error message is printed to the Serial console
2. The command is ignored (no action taken)
3. Previous settings remain unchanged

---

## Firmware Requirements

| Parameter | Value |
|-----------|-------|
| **Minimum firmware version** | 4.0 |
| **Serial baudrate** | 115200 |
| **NMEA0183 baudrate** | 4800 |
| **Default Wi-Fi port** | 10110 (configurable) |
| **BLE MTU** | 100–500 bytes |
| **NMEA2000 PGN** | 743837 (proprietary) |


