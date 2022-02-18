#ifndef PERIPUMP_H
#define PERIPUMP_H

#include "control.h"
#include "screen.h"

void motor_stop(Screen *display, const Motor *motor);
void motor_forward(Screen *display, const Motor *motor, byte dutyCycle);
void motor_reverse(Screen *display, const Motor *motor, byte dutyCycle);
void motor_check(Screen *display, Assem *assem);

#endif