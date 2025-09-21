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
