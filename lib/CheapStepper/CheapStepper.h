#ifndef _CHEAP_STEPPER_SENSOR_H
#define _CHEAP_STEPPER_SENSOR_H

#ifndef ARDUINO
#include <stdint.h>
#elif ARDUINO >= 100
#include "Arduino.h"
#include "Print.h"
#else
#include "WProgram.h"
#endif


#define CHEAP_STEPPER_STEPS_PER_ROTATION 4095;

class CheapStepper {
public:
  // Constructor(s)
  CheapStepper();
  CheapStepper(int,int,int,int);
  virtual ~CheapStepper() {}
  void setup();
  void move_stepper(int step_count);
  void toggleDirection();
  void stop();
  void setDirection(int);
private:
  boolean step_positions[4][4] = {
      { 0,0,1,1 },
      { 0,1,1,0 },
      { 1,1,0,0 },
      { 1,0,0,1 },
  };
  byte sbt0811_in[4] = {25,26,27,14};   // IN1, IN2, IN3, IN4 to arduino pins

  int full_steps_count = 2038;    // steps for a full cycle
  int step_time        = 1;       // 1000us for one step

  int direction    = 1;           // will be either 1 (clockwise) or -1 (anti-clockwise)
  int steps_left   = full_steps_count;
  int current_step = 0;           // 0 to 7
};

#endif
