//MOTOR
//#include <analogWrite.h>


#define RPWM 33
#define LPWM 32
#define CS 35 //current senseo
int counter = 0;
long prevMillis = 0;
int startSpeed = 255;
float current = 0;

//overcurrent
int max_current = 2000; //should be 500

bool end_left = false;
bool end_right = false;

//for pulsing motor
bool motor_running = false;
long motor_state_millis = 0;//to check how long the motor is in the current state, on or off

void motor_setup(){
    // put your setup code here, to run once:
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);
  Serial.println("motor setup");
}

void motor_loop(){
  

 
  if(PID_command == 0){
    motor_stop();
    return;
  }

  int motor_speed = abs(PID_command);
  int motor_direction = 1;
  if(PID_command<0)motor_direction = 0;
  
  

  
  //DC motors do not perform well on 50% duty cycle so we are going to pulse instead.
  if (motor_speed < 150){
    //calculate maximum motor on time //it will be a percentage of 5.000 seconds.
    int motor_on_limit = map(motor_speed,0,122,0,5000);
    //check if the maximum time exceeded and then stop
    if(motor_running && millis() - motor_state_millis > motor_on_limit){
      motor_stop();
      return;
    }else if(!motor_running && (millis() -  motor_state_millis) < (5000 - motor_on_limit)){
      motor_stop();
      return ;
    }

    motor_speed = 150;//minium motor speed
  }

  motor_set(motor_speed,motor_direction);

}

void motor_set(int motor_speed, int motor_direction){

  Serial.print("motor_set ");
  Serial.println(motor_speed);
  if(motor_speed == 0){
    motor_stop();
  }

    //check for the maximum motor speed
  if(motor_speed > 255)motor_speed = 255;

 
   //analog write has a limit of 255. 
  //Also, DC motors only have a good speed at 50% PWM = 122. May be lower for some motors.
  
  //First check if we are over the current limmit
  current = current*0.99+analogRead(CS)*0.01;

 
  if(motor_direction == 1){
    if(end_left || current > max_current){
      Serial.print(current);
      Serial.println(" current too high end_left reached");
      end_left = true;
      motor_stop();
      return;
    }

     if(end_right && motor_speed>0)end_right = false; //if we were at the right end, and now turning left  again, than end_right is false;
     
     analogWrite(LPWM, 0);
     analogWrite(RPWM, motor_speed);
     
  }else{ // motor direct = 0
    if(end_right || current > max_current){
      Serial.println("current too high end_right reached");
      end_right = true;      
      motor_stop();
      return;
    }

    if(end_left && motor_speed>0)end_left = false; 
    
    analogWrite(RPWM, 0);
     analogWrite(LPWM, motor_speed);
     
  }

  if(!motor_running){
    motor_running = true;
    motor_state_millis = millis();
  }
}

void motor_stop(){
  analogWrite(LPWM, 0);
  analogWrite(RPWM, 0);
  //motor_speed = 0;
  if(motor_running){
    motor_running = false;
    motor_state_millis = millis();
    
  }
  
     
}

/*
void print_motor(){
  Serial.print("motor_speed:");
  Serial.print(motor_speed);
  Serial.print(",PID_command:");
  Serial.print(PID_command);
  Serial.print(",motor_direction:");
  Serial.print(motor_direction);
  Serial.print(",current:");
  Serial.print(current);

  
  Serial.println();
}
*/
