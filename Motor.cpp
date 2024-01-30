// Motor.cpp
#include "Motor.h"
#include <Arduino.h>


const int RPWM_PIN = 33;
const int LPWM_PIN = 32;

bool end_left = false;
bool end_right = false;



//overcurrent
float Motor::current = 0;
int Motor::max_current = 2000; //should be 500


bool Motor::running = false;
unsigned long Motor::state_millis = 0;

void Motor::setup() {
  pinMode(RPWM_PIN, OUTPUT);
  pinMode(LPWM_PIN, OUTPUT);
  analogWrite(RPWM_PIN, 0);
  analogWrite(LPWM_PIN, 0);
  Serial.println("motor setup");
}

void Motor::loop() {
  // Your motor loop logic here
  // For example, you can call motor_set and other motor-related functions here
}

void Motor::set(int motor_speed, int motor_direction) {
  Serial.print("motor_set ");
  Serial.println(motor_speed);

  if (motor_speed == 0) {
    stop();
  }

   //check for the maximum motor speed
  if(motor_speed > 255)motor_speed = 255;

 
   //analog write has a limit of 255. 
  //Also, DC motors only have a good speed at 50% PWM = 122. May be lower for some motors.
  
  //First check if we are over the current limmit
  //current = current*0.99+analogRead(CS)*0.01;

 
  if(motor_direction == 1){
    if(end_left || current > max_current){
      Serial.print(current);
      Serial.println(" current too high end_left reached");
      end_left = true;
      Motor::stop();
      return;
    }

     if(end_right && motor_speed>0)end_right = false; //if we were at the right end, and now turning left  again, than end_right is false;
     
     analogWrite(LPWM_PIN, 0);
     analogWrite(RPWM_PIN, motor_speed);
     
  }else{ // motor direct = 0
    if(end_right || current > max_current){
      Serial.println("current too high end_right reached");
      end_right = true;      
      Motor::stop();
      return;
    }

    if(end_left && motor_speed>0)end_left = false; 
    
    analogWrite(RPWM_PIN, 0);
     analogWrite(LPWM_PIN, motor_speed);
     
  }

  if(!Motor::running){
    Motor::running = true;
    Motor::state_millis = millis();
  }
}

void Motor::stop() {
  analogWrite(LPWM_PIN, 0);
  analogWrite(RPWM_PIN, 0);

  if (Motor::running) {
    Motor::running = false;
    Motor::state_millis = millis();
  }
}

// Define other functions like motor_loop, motor_set, and other helper functions as needed
