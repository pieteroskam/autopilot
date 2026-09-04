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
```

---

# ⚓ Command API Documentation

## Table of Contents
- [Overview](#overview)
- [Communication Channels](#communication-channels)
- [Command Format](#command-format)
- [Command Reference](#command-reference)
- [Enumerations](#enumerations)
- [Route Following & Chartplotter Integration](#route-following--chartplotter-integration)
- [BLE PIN Gate](#ble-pin-gate)
- [Parser Behavior](#parser-behavior)
- [Firmware Requirements](#firmware-requirements)

---

## Overview

All commands — from Serial, Wi-Fi, NMEA0183, NMEA2000, or Bluetooth LE — are processed through a single unified command handler:

```cpp
void processCommand(const std::string& rxString, CommunicationSource fromSource);
```

Text commands are CSV-like strings; the binary channels (NMEA2000 bus and the BLE N2K characteristic) are decoded into the same string form before dispatch, so every channel supports the same command set.

Any string starting with `$` is routed to the NMEA0183 parser instead (so you can inject NMEA sentences over any text channel, including BLE and Serial).

---

## Communication Channels

| Channel | Protocol | Description | Example |
|---------|----------|-------------|---------|
| **Serial** | UART @ 115200 baud | Debug or direct control | `3,0` |
| **NMEA0183 (wired)** | RS-422 @ 4800 baud | Commands wrapped in an `$APCMD` sentence; standard sentences (RMC, MWV, APB, RSA) are parsed directly | `$APCMD,3` |
| **NMEA0183 (Wi-Fi)** | UDP, port 10110 (configurable) | Same sentences as wired NMEA0183 | `$GPRMC,...` |
| **NMEA2000** | Proprietary PGN **130971** | Nautinect autopilot control PGN (see binary format below). PGN 130970 = status broadcast, 130972 = setting values | — |
| **BLE text channel** | GATT write, characteristic `4ac8a682-9736-4e5d-932b-e9b31405049c` | Used by the mobile app for text commands. Responses/notifications arrive on `0438da27-ea40-461a-b9bc-f486f40f401c` | write `"3,0"` |
| **BLE N2K channel** | GATT write/notify, characteristic `47ca08d8-ae18-4e2e-9629-8591474e1153` | Binary pipe carrying NMEA2000 frames: `[PGN lo][PGN mid][PGN hi][payload]`. **Firmware 4.4.1+** feeds incoming frames through the same PGN parser as the physical NMEA2000 bus, so standard PGNs (129283, 129284, 129025, 129026, 130306, 127250, …) work over BLE too. Status/settings PGNs are notified back on the same characteristic | — |

Service UUID (all BLE characteristics): `ab0828b1-198e-4351-b779-901fa0e0371e`

---

## Command Format

### Text form (Serial, Wi-Fi, `$APCMD`, BLE text channel)

```
<CommandID>,<value>[,<value2>][,<value3>]
```

Examples:

```
3,0            → ap_on (and switch heading source to 0)
2,180          → ap_target (target heading = 180°)
43,235.0,4.20,52.545275,4.925772 → phone GPS: cog, sog (knots), lat, lon
motor200       → legacy raw motor command (speed −255…255)
```

### Binary form (NMEA2000 PGN 130971, and the same frame tunneled over the BLE N2K channel)

Payload layout of PGN 130971:

```
[manufacturer code: uint16 LE = 2046] [subID: 1 byte] [data...]
```

- **subID 2** — data is a NMEA2000 var-string containing the text command, e.g. `"3,0"`.
- **subID 3** — compact binary command: `[CommandID: 1 byte]` followed by
  - one or two `int16` (little-endian) values for numeric commands,
  - a single byte for `get_setting` (252),
  - a zero-terminated string for string commands (`wifi_ssid`, `wifi_pass`).

On the BLE N2K characteristic the frame is prefixed with the 3-byte PGN (little-endian):

```
[0x9B 0xFF 0x01] [manufacturer LE] [subID] [data...]     (0x01FF9B = 130971)
```

---

## Command Reference

### Autopilot control
| ID | Command | Description | Parameters |
|----|---------|-------------|------------|
| **1** | `motor_set` | Set motor speed directly | `int speed` (−255…255), `int duration_ms` _(optional)_ |
| **2** | `ap_target` | Set target heading | `int heading_deg` (0–359) |
| **3** | `ap_on` | Engage autopilot | `int headingSource` _(optional, see HeadingSource)_ |
| **4** | `ap_off` | Disengage autopilot | _(none)_ |
| **6** | `heading_source` | Change heading input source | `int source` (see HeadingSource) |
| **50** | `target_heading_add` | Adjust target heading by a delta | `int degrees` (signed) |
| **51** | `tack` | Perform a tack | `1` = starboard, `0` = port |
| **41** | `set_tack_angle` | Set sailing tack angle | `int degrees` |
| **44** | `minor_degree_change` | Degrees per +1/−1 button press | `uint8_t degrees` |

### Compass & calibration
| ID | Command | Description | Parameters |
|----|---------|-------------|------------|
| **7** | `calibrate_gyro` | Start gyroscope calibration | _(none)_ |
| **8** | `mag_continuous` | Continuous magnetometer calibration | `1` = on, `0` = off |
| **9** | `calibrate_mag` | Start magnetometer calibration | _(none)_ |
| **10** | `mag_default` | Reset magnetometer to default calibration | _(none)_ |
| **11** | `get_mag_cal_backup` | Read magnetometer backup from EEPROM | _(none)_ |
| **12** | `set_mag_cal_backup` | Save current calibration as backup | _(none)_ |
| **13** | `set_compass_heading` | Manually adjust compass offset | `int new_heading_deg` |
| **35** | `get_calibration` | Print current calibration info | _(none)_ |
| **45** | `calibrate_gyro_mag_single_point` | Single-point calibration against a known course | `cog_deg, fieldStrength_µT, inclination_deg, declination_deg` |
| **46** | `calibration_progress` | _(autopilot → display)_ calibration coverage broadcast | 36-char segment string |

### PID configuration
| ID | Command | Description | Parameters |
|----|---------|-------------|------------|
| **14** | `gain_overall` | Overall PID gain | `int gain` |
| **15** | `gain_p` | Proportional gain | `int gain` |
| **16** | `gain_i` | Integral gain | `int gain` |
| **17** | `gain_d` | Derivative gain | `int gain` |
| **18** | `gain_dd` | Second-derivative gain | `int gain` |
| **19** | `gain_ff` | Feed-forward gain | `int gain` |
| **20** | `precision` | Dead-band / precision | `int value` |

### Motor & rudder configuration
| ID | Command | Description | Parameters |
|----|---------|-------------|------------|
| **22** | `min_motor_speed` | Minimum motor speed | `int speed` (0–255) |
| **23** | `max_motor_speed` | Maximum motor speed | `int speed` (0–255) |
| **24** | `motor_running_limit_enabled` | Enable motor running limit | `1` / `0` |
| **25** | `motor_running_limit` | Motor running time limit | `int seconds` |
| **26** | `motor_reverse` | Reverse motor direction | `1` / `0` |
| **27** | `motor_max_current` | Maximum motor current | `int` (0–4096) |
| **28** | `pulse_interval` | Pulse feedback interval | `int ms` |
| **29** | `pulse_mode` | Pulse feedback mode | `int mode` |
| **30** | `motor_type` | Motor type | `int type` |
| **36** | `rudder_feedback_type` | Rudder feedback sensor type | `0` = none, `1` = voltage, `2` = pulse |
| **37** | `rudder_center_position` | Rudder center position | `int16` |
| **38** | `rudder_left_position` | Rudder left limit | `int16` |
| **39** | `rudder_right_position` | Rudder right limit | `int16` |
| **40** | `rudder_pulse_count` | Rudder pulse count | `int16` |
| **42** | `rudder_range` | Maximum rudder angle range | `int degrees` |
| **62** | `rudder_command` | Rudder demand from an external control loop (position with feedback sensor, time-based pulse without) | `int16` |

### GPS & route
| ID | Command | Description | Parameters |
|----|---------|-------------|------------|
| **43** | `phone_gps_data` | Phone GPS fix (app → autopilot) | `cog_deg, sog_knots, lat, lon` |
| **48** | `nmea_gps_fix` | _(autopilot → app)_ onboard GPS fix broadcast, 1 Hz | `lat, lon, cog, sog, xte` |
| **60** | `set_waypoint` | _(legacy)_ single waypoint for local heading-to-steer computation. Prefer sending APB / PGN 129284 instead | `lat, lon` |
| **61** | `xte_gain` | XTE correction gain applied in TRACK mode | `uint8_t` (1–100, degrees per 100 m XTE, default 17) |

### Network & NMEA
| ID | Command | Description | Parameters |
|----|---------|-------------|------------|
| **31** | `wifi_enable` | Wi-Fi mode | `int mode` |
| **32** | `wifi_ssid` | Wi-Fi SSID | `string` |
| **33** | `wifi_pass` | Wi-Fi password | `string` |
| **34** | `wifi_port` | UDP port for NMEA over Wi-Fi | `int port` |
| **49** | `get_nmea_state` | Report NMEA0183/2000/Wi-Fi RX/TX counters | _(none)_ |
| **70** | `nmea_emulation_mode` | Emulate a commercial pilot on the NMEA2000 bus | `0` = off, `1` = Raymarine, `2` = Navico (B&G/Simrad/Lowrance) |

### Bluetooth access (PIN gate)
| ID | Command | Description | Parameters |
|----|---------|-------------|------------|
| **71** | `pairing_mode` | BLE access mode | `0` = open, `1` = PIN on phone channels, `2` = PIN on all BLE channels |
| **72** | `ble_login` | Log in for this BLE connection | `pin` (0–9999) |
| **73** | `ble_pin` | Store a PIN (enables mode 1 if the gate was off) | `pin` (0–9999) |
| **74** | `auth_state` | _(autopilot → app)_ lock status | `0` = locked, `1` = unlocked |

### System
| ID | Command | Description | Parameters |
|----|---------|-------------|------------|
| **21** | `log_values` | Enable/disable value logging stream | `1` / `0` |
| **47** | `available_sources` | _(autopilot → app)_ bitmask of live heading sources | bit N = HeadingSource N |
| **249** | `restore_defaults` | Reset stored settings to factory values and restart. Leaves calibration and wiring settings untouched | `0` = PID + motor, `1` = PID, `2` = motor |
| **250** | `restart` | Restart the autopilot | _(none)_ |
| **252** | `get_setting` | Request one setting; answered as `<id>,<value>` | `int commandId` |
| **253** | `send_settings` | Send all stored settings to the app | _(none)_ |
| **254** | `protocol_version` | Return the protocol version (currently 3) | _(none)_ |

---

## Enumerations

### HeadingSource

```cpp
enum HeadingSource {
  autopilotCompass = 0,
  phoneCompass     = 1,
  phoneGps         = 2,
  AWA              = 3,
  TWA              = 4,
  NMEA_GPS         = 5,
  NMEA_Compass     = 6,
  TRACK            = 7
};
```

| Value | Source | Description |
|-------|--------|-------------|
| **0** | `autopilotCompass` | Internal compass (sensor unit) |
| **1** | `phoneCompass` | Smartphone compass; the phone runs the control loop and sends motor/rudder commands |
| **2** | `phoneGps` | Smartphone GPS heading; the phone runs the control loop |
| **3** | `AWA` | Apparent Wind Angle (wind sensor) |
| **4** | `TWA` | True Wind Angle |
| **5** | `NMEA_GPS` | GPS course from NMEA0183/NMEA2000 (or phone GPS via command 43) |
| **6** | `NMEA_Compass` | External compass (PGN 127250) |
| **7** | `TRACK` | Route following: steer to a waypoint provided by a chartplotter or the phone app |

Sources 3–7 announce themselves when data arrives and expire after 10 s of silence; command 47 broadcasts which sources are currently usable.

---

## Route Following & Chartplotter Integration

In **TRACK** mode the autopilot behaves like a pilot connected to a chartplotter: the navigator (chartplotter **or** the phone app acting as one) computes the cross-track error and bearing to the active waypoint, and the autopilot steers `bearing + xte_gain × XTE` (command 61 sets the gain, clamped to ±45°).

Any of these inputs drive TRACK mode — send at ~1 Hz:

| Input | Channel | Content |
|-------|---------|---------|
| `$--APB` sentence | NMEA0183 (wired, Wi-Fi, or any text channel) | XTE + heading to steer |
| PGN **129283** | NMEA2000 bus or BLE N2K channel | Cross-track error (positive = boat starboard of track) |
| PGN **129284** | NMEA2000 bus or BLE N2K channel | Distance + bearing to the active waypoint |

Position and course for the autopilot's own heading come from (first available wins):

| Input | Channel | Content |
|-------|---------|---------|
| `$--RMC` sentence | NMEA0183 | Position, SOG, COG |
| PGN **129025** + **129026** | NMEA2000 bus or BLE N2K channel | Position rapid + COG/SOG rapid |
| Command **43** | BLE text channel | Phone GPS: `cog, sog (knots), lat, lon` |

While an onboard GPS is alive the autopilot broadcasts its fix to the app (command 48) and the app suspends its own GPS; when the phone supplies GPS via command 43, the fix broadcast is suppressed so the app never mistakes its own position for an onboard GPS (firmware 4.4.1+).

Waypoint advancement (switching to the next leg) is the navigator's job — the autopilot only steers the active leg, exactly like with a commercial chartplotter.

---

## BLE PIN Gate

Optional app-level access control (firmware 4.4.0+), configured with commands 71–73:

- **Mode 0** (default): open — existing installations are unaffected.
- **Mode 1**: phone channels require `ble_login` (72) per connection; the display/N2K channel stays open.
- **Mode 2**: all BLE channels require login, including binary commands and nav PGNs on the N2K characteristic.

Three failed logins disconnect the client. Wired channels (Serial, NMEA0183, NMEA2000) are never gated. Holding the BOOT button (GPIO 0) for 5 seconds clears the PIN and mode.

---

## Parser Behavior

- All text values are parsed with `std::stoi()` / `std::stof()` based on the expected parameter type.
- Invalid values are ignored; an error message goes to the Serial console and previous settings remain unchanged.
- Valid settings are automatically saved to EEPROM and persist across reboots.
- Legacy commands (`motor<speed>`, `apOn`, `apOff`, …) remain supported for old app versions.

---

## Firmware Requirements

| Parameter | Value |
|-----------|-------|
| **Current firmware version** | 4.4.1 |
| **Protocol version** (command 254) | 3 |
| **Standard PGNs over the BLE N2K channel** | firmware ≥ 4.4.1 (older firmware treats every frame on that characteristic as a binary command — do not send nav PGNs to it) |
| **BLE PIN gate** | firmware ≥ 4.4.0 |
| **Serial baudrate** | 115200 |
| **NMEA0183 baudrate** | 4800 |
| **Default Wi-Fi UDP port** | 10110 (configurable) |
| **NMEA2000 proprietary PGNs** | 130970 (status), 130971 (commands), 130972 (settings) |
