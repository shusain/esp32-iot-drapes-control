#include "CheapStepper.h"
/*
https://www.electronicoscaldas.com/datasheet/ULN2003A-PCB.pdf

   SBT0811 + 28BYJ-48 stepper motor
   https://www.youtube.com/watch?v=BBVmeim2uu8
   Connect :
       IN1 -> D8
       IN2 -> D9
       IN3 -> D10
       IN4 -> D11
       VCC -> 5V-12V external source (+)
       GND -> 5V-12V external source (-)
*/
/*!
 *  @brief  class constructor
 */
CheapStepper::CheapStepper() {}
CheapStepper::CheapStepper(int pin1, int pin2, int pin3, int pin4) {
    sbt0811_in[0] = pin1;
    sbt0811_in[1] = pin2;
    sbt0811_in[2] = pin3;
    sbt0811_in[3] = pin4;
}

void CheapStepper::setup() {
    // Serial.println("CheapStepper setup");
    for (byte i = 0; i <= 3; i++) {
        pinMode(sbt0811_in[i], OUTPUT); 
    }
}



void CheapStepper::move_stepper(int step_count) {
    // Serial.println("CheapStepper moving");
    for (int i=0;i<step_count;i++) {
        // step one step in given direction
        current_step += direction;
        // current_step = current_step % 7;
        if (current_step > 3) { current_step = 0; }
        if (current_step < 0) { current_step = 3; }

        // set digital pins for the current_step
        for (byte i = 0; i <= 3; i++) {
            digitalWrite(sbt0811_in[i], step_positions[current_step][i]); 
        }
        
        delay(step_time);
    }
} 
void CheapStepper::toggleDirection() {
    // Serial.println("CheapStepper toggling direction");
    direction  = (direction > 0 ? -1 : 1);
}
void CheapStepper::stop() {
    // Serial.println("CheapStepper toggling direction");
    for (byte i = 0; i <= 3; i++) {
        digitalWrite(sbt0811_in[i], 0); 
    }
    current_step=0;
}
void CheapStepper::setDirection(int direction) {
    // Serial.println("CheapStepper direction set");
    this->direction  = direction;
}
