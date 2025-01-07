# Installation guide
https://youtu.be/zypO15Sglec
https://nautinect.com/buildyourownautopilot

# Shop Nautinect.com
On https://nautinect.com you can order  DIY packages including a pre programmed microcrontoller. Also the Full Features pilot can be bought including wireless remote, current ensing and onboard compasss can be bought.

# Facebook group.
Please, ask question in the facebook group so others can answers and answers will be seen by everyone. Facebook is not my prefered medium but I didn't know a better one. Please join here: https://www.facebook.com/groups/721879886302423



# autopilot
ESP32 autopilot BLE

This code can be flashed on an ESP32 using the Arduino SDK.
It can be used as a standalone autopilot based on compasss steering. Another method is to download the app:
https://play.google.com/store/apps/details?id=com.sailtactical.autopilot
https://apps.apple.com/nl/app/nautinect-autopilot/id6473739289

By downloading this app, the compass, gyro and GPS of the phone will be used. In that case, you only need an ESP32 and a motor driver. Wire the motor driver to the ESP, Connect VCC and Ground as per documentation. Connect pin 33 to motor-driver-left and pin 32 to motor-driver-right. It is

## install the ESP32 driver
If your computer doesnt recognize the device when you plug it in, you need to install the CP210x driver. It can be downloaded here:
https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads



## For use in the arduino SDK V2. 

1. Download and install Arduino IDE V2 https://www.arduino.cc/en/software
2. Go to tools->boards manager and search for esp32. Install the esp32 by Espressif Systems.
3. Create a folder named autopilot_basic.
   - In this folder, Put all the files (with .ino, .cpp and .h extension) from the repository.
   - Open autopilot_ble.ino with the arduino IDE
4. Select your board (My case DEVKIT ESP32) and COM port. If the ESP boards cant be selected. please add ESP to the arduino IDE
Run

Now open te app on your phone. It will search for a bluetooth device called autopilot. When its found, connect to it.

## Hardware
1. ESP32. I prefer an USB-C type with 32 pins (CH340C)
2. Motor driver. IBT-2 (max 40Amps) or the L298n (max 2 Amps).
   - Both motor drivers have many clones and they can behave differently. the +5V power supply isnt always available. Faulty devices are common. So make sure to order atleast 2. 
4. Jumper wires
5. 5V supply to ESP32. You have several options
   - Use a powerbank,
   - usb socket
   - +5V from L298n
   - Voltage regulator to bring the 12v/24v down to 5v.
   - Phone with USB OTG cable.  
  
## Wiring
The IBT-2 has 8 pins and we use 6  
1 VCC --> +5Volts from ESP32  
2 GND --> GND from ESP32  
3 R_IS --> Not connected  
4 L_IS --> Not connected  
5 R_EN --> +5V from esp32  
6 L_EN --> +5V from esp32  
7 R_PWM --> GPIO 32 on ESP32  
8 L_PWN --> GPIO 33 on ESP32  

# App settings 

The gains are as follows:  
  
P - proportional - heading error  
I - integral - based on the accumulated error  
D - derivative - rate of turn  
DD - derivative' - rate of rate of turn  
PR - proportional root - square root of heading error  
FF - feed forward - change in heading command  


To get started retuning from scratch (or on a new boat) set all of the gains to zero, except the P and D gains. It is possible to have a fully usable (but less efficient) autopilot using only these two gains.

Set the P gain to a low value (say 5) and the D gain to 15. Typically on larger boats, you will need higher values, but it really depends on how fast the drive motor turns the rudder.

The hard over time is how long it takes to turn the rudder from end stop to end stop. This is typically 30 degrees for each side. If a smaller motor is geared down more, and takes, say 16 seconds, then these gains should be doubled  as a starting point.

If the boat takes too long to correct the course and spends a long time to one side of the correct heading, increase these two gains. If the motor is working too hard, and frequently crosses the correct heading, decrease these gains.

P - proportional gain This value should normally be set low. If it is set too high, the boat will constantly turn across the desired heading. If it is too low, the boat may fail to maintain course. As it is increased a higher D gain is needed to compensate (prevent overshoot)

D - derivative gain This is the gyro gain, and the main driving gain of the autopilot. Most of the corrections should be as a result of this gain. Once the best value is found it can typically work in a range of conditions, however, in light air, it can be reduced (along with reducing other gains) to significantly reduce power consumption especially if the boat is well balanced.

PR - proportional root gain This gain can be really useful preventing oscillation especially upwind. To use it, increase it until it takes effect, and gradually back off on the P gain. You will still need some P gain, but it may be less than half of before if a sufficient PR gain is used.

DD - derivative' gain This gain is useful to improve reaction time. It can allow for corrections sooner than they would occur from the D gain alone. To use it, gradually increase this value up to 1.5x the D gain value without changing other gains, and compare the results.

FF - feed forward gain This gain is only useful when making course changes. For holding heading it has no effect. Following a route can cause course changes. It can be very useful in improving the response time since a low P value is normally desirable, this gain is the main contributor when the course is adjusted.

I - integral gain This gain does not need to be used to hold a course, however it can compensate if the actual course held is different from the commanded course. If following routes, and the boat tends to follow along a line parallel to the route, this will compensate for that error. It is best to start at zero, and very carefully increase it until the results are improved. If the value is too high, it will simply increase power consumption.
