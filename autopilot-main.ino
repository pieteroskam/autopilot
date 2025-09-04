/*
MIT License

Copyright (c) 2024 Pieter Oskam

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software for personal use only. Any distribution to other persons, commercialy or not is prohibited.

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Bluetooth.h"
#include "Motor.h"



void setup() {

  Serial.begin(115200);


    
    delay(1000);
    
  
  //if(compass_enable)compass_setup();
  //ota_wifi_setup();
  //button_setup();
  //display_setup();
  Motor::setup();
  delay(1000);
  Bluetooth::setup();
  
  Serial.println("setup complete"); 

  delay(1000);

}

void loop(){
  
    
} ///END LOOP


