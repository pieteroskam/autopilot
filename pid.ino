float max_I = 10;

////VARIABLES/////
int loop_time = 100; //compute PID each X miliseconds

float heading_target = 0;
float error = 0;// the heading error . If heading is 10 and set =5 then errpr is 5
float error_I = 0;// integeral heading error

float heading_d_previous;
float loop_count = 0;
float loop_error_sum = 0;

float PID_process(){

    float dt = float(millis() - prev_pid_ms) / 1000.0;
    prev_pid_ms = millis();
  

  
  

    //The error, also called P (proportional). So if target is 20 and acutal value is 10. P is -10
    error = headingDiff(heading_float,heading_target);  //heading_float-heading_target

    //heading_D is derived from the Gyro -1 means turning -1 degree per second
    //error_DD is second derivatie of heading based on heading_D. -1 means an accelrations of -1 degree per second
    
  
    P = error*gain_P;;
    D = 0;
    DD = 0;
    I = 0;
    

    if(gain_D>0) D =  heading_d*gain_D;

    if(gain_DD>0) DD =  (heading_d-heading_d_previous)/dt*gain_DD;
    heading_d_previous = heading_d;

    if(gain_I>0){
      error_I += error*dt;
      error_I = minmax(error_I, max_I, -max_I);
      I = error_I*gain_I;
    }
    
    PID_command = P+I+D+DD;

  
   print_pid();
    
     
}

void print_pid(){
     Serial.print("PID_command:");
     Serial.print(PID_command);
     Serial.print(",error:");
     Serial.print(error);     
     Serial.print(",P:");
     Serial.print(P);
     Serial.print(",I:");
     Serial.print(I);
     Serial.print(",D:");
     Serial.print(D);
     Serial.print(",DD:");
     Serial.println(DD);
}


void PID_setTarget(int target){
  heading_target = target;
}
