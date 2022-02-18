#include "peripump.h"

byte x_offset = 46;

// Turns off a motor
void motor_stop(Screen *display, const Motor *motor)
{
    if (motor->id == 1)
    {
        (*display).setCursor(x_offset, 8);
    }
    else
    {
        (*display).setCursor(x_offset, 24);
    }
    (*display).println("O");
    off_motor(motor);
}

// Drives the motor forward at duty cycle 'dutyCycle'
void motor_forward(Screen *display, const Motor *motor, byte dutyCycle)
{
    if (motor->id == 1)
    {
        (*display).setCursor(x_offset, 8);
    }
    else
    {
        (*display).setCursor(x_offset, 24);
    }
    (*display).println("F");
    digitalWrite(motor->pin1, HIGH);
    digitalWrite(motor->pin2, LOW);
    analogWrite(motor->enable, dutyCycle);
}

// Drives the motor in reverse at duty cycle 'dutyCycle'
void motor_reverse(Screen *display, const Motor *motor, byte dutyCycle)
{
    if (motor->id == 1)
    {
        (*display).setCursor(x_offset, 8);
    }
    else
    {
        (*display).setCursor(x_offset, 24);
    }
    (*display).println("R");
    digitalWrite(motor->pin1, LOW);
    digitalWrite(motor->pin2, HIGH);
    analogWrite(motor->enable, dutyCycle);
}

// Logic to check motor switch state
void motor_check(Screen *display, Assem *assem)
{

    if (digitalRead(assem->control->fwd) == LOW)
    {
        assem->control->flag = 1;
        motor_forward(display, assem->motor, assem->flow->dutyCycle);
    }
    else if (digitalRead(assem->control->rev) == LOW)
    {
        assem->control->flag = 2;
        motor_reverse(display, assem->motor, assem->flow->dutyCycle);
    }
    else
    {
        assem->control->flag = 0;
        motor_stop(display, assem->motor);
    }
}