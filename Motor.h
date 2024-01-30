// MotorControlModule.h
#ifndef MOTOR_H
#define MOTOR_H

class Motor {
public:
  static void setup();
  static void loop();
  static void set(int motor_speed, int motor_direction);
  static void stop();
  static float current;
  static int max_current;

private:
   static const int CS_PIN = 35;
  static int counter;
  static long prevMillis;
  static int startSpeed;

  static bool running;
  static unsigned long state_millis;
};

#endif
