#ifndef CONTROL_H
#define CONTROL_H

#include <Arduino.h>
#include "motor_structs.h"

// 3rd order polynomial coefficients for flow rate calibration
struct Calib
{
    const float a;
    const float b;
    const float c;
    const float d;
};

typedef struct Calib Calib;

void init_switch(Control *control);
void init_motor(const Motor *motor);
void off_motor(const Motor *motor);
float conv_flow_rate(byte dutyCycle, byte flag, byte id);
float get_flow_rate(float dutyCycle_f, Calib *calib);
void control_flow(Assem *assem);

#endif