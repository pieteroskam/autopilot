//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

/////////OLED/////////////////
//#include "Arduino.h"

//General settings
boolean bluetooth_enable = 0;
boolean compass_enable = false;
boolean screen_enable = false;


//display
void display_loop(String);
String display_message = "";

//Menu setting
//selection mode for setting 0=off 1=gain_P 2=gain_I 3=gain_D 4=gain_DD
int mode_selected = 0;

//loop times
static uint32_t prev_screen_ms = 0;
static uint32_t prev_pid_ms = 0;
float PID_command = 0;
int min_pid_command = 10;

//GAINS these are to tweak the PID controller. The defaults are quitte agressive.
float gain_P = 5; // max 0.02 // heading error proportional
float gain_I = 0; // max .1)   # integral
float gain_D = 20; // max .8)   # derivative (gyro Z)
float gain_DD = 0 ;

//PID VALUES


float P;
float D;
float DD;
float I = 0;

String message = "Hello!";
unsigned long message_time = 0;
const int message_duration = 5000;



//General
boolean ap_on = false; //is the auto pilot active?

float heading_float;


int heading_command; //The heading that the sailor set the AP to!
float heading_error;// the heading error. If heading is 10 and heading_set =5 then errpr is 5
float hedding_error_integeral;// integeral heading error
float heading_rate; //the heading change in a certain time period. (derivative)
float hading_rate_rate;// the rate of heading change. (derivate of derivative)
float heading_command_rate; //FF Feed Forward. To estimate the rate going to happen 
int rate = 100; //Hz Sample Rate This is how often we check the compass, ToDo check this

//define functions here so that bluetooth has acces to it.
void apToggle();
void motor_set(int,int);

#include "math_utils.h"
#include <math.h>
//#include "ota_wifi.h"
#include "bluetooth.h"



void setup() {
//Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);  
  Serial.begin(115200);
    
    delay(1000);
    display_setup();
  button_setup();
  if(compass_enable)compass_setup();

//  ota_wifi_setup();
  
  
  motor_setup();
  bluetooth_setup();
  
  Serial.println("setup complete"); 


  ////apToggle();
  
}

void loop(){
  button_loop();
  
  ///////////////s128*64//////////////////
  //compass_loop();
  if(screen_enable && millis() > prev_screen_ms + 500){
    display_loop("");
    prev_screen_ms = millis();              

  }

      if(millis() - prev_pid_ms > 100){
        if(ap_on == 1){
          PID_process();
          if(abs(PID_command)<min_pid_command)PID_command = 0;    
          motor_loop(); 
          bluetooth_loop();         
        }else{
          PID_command = 0;
          
        }
      }


 // ota_wifi_loop();
  
  
    
} ///END LOOP





void change_heading_command(int diff){
  //Check if we are in mode selection.
  if(mode_selected){
    if(mode_selected==1){
      gain_P += diff;
    } else if(mode_selected==2){
      gain_I += diff;
    } else if(mode_selected==3){
      gain_D += diff;
    } else if(mode_selected==4){
      gain_DD += diff;
    }
    return;//dont change the heading command
  }
  
  heading_command += diff;
  if(heading_command>=360) heading_command -=360;
  if(heading_command<0) heading_command +=360;
  PID_setTarget(heading_command);  

  display_loop("");
  Serial.println("Heading command changed to"+String(heading_command));

}

void msg(String msg){
  message = msg;
  message_time = millis();  
}

//turns the AP on and off
void apToggle(){
  Serial.print("apToggle() ");
  Serial.print(ap_on);
  Serial.print(">");
  
    
    if(ap_on){
      ap_on = false;
      motor_stop();
      msg("AP off");
    }else{
       ap_on = true;
       heading_command = heading_float;
       change_heading_command(0);
      msg("AP ON");
    }
    Serial.println(ap_on);     
}


//NOTES TO SELLF
/*
DLPF = DIGITAL LOW PASS FILTER

*/
