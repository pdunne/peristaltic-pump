#include <Arduino.h>
#include "control.h"

//TODO: Dummy variables, need to be measured and updated
Calib fwdA = {0.0, 0.0, 0.8, -133.0};
Calib revA = {0.0, 0.0, 0.81, -139.0};
Calib fwdB = {0.0, 0.0, 0.82, -135.0};
Calib revB = {0.0, 0.0, 0.8, -132.0};

// Initialises pullup pins for switch detection
void init_switch(Control *control)
{
    pinMode(control->fwd, INPUT_PULLUP);
    pinMode(control->rev, INPUT_PULLUP);
}

// Intialises pins for controlling a motor
void init_motor(const Motor *motor)
{
    pinMode(motor->enable, OUTPUT);
    pinMode(motor->pin1, OUTPUT);
    pinMode(motor->pin2, OUTPUT);
    digitalWrite(motor->enable, LOW);
}

// Turns off a motor
void off_motor(const Motor *motor)
{
    digitalWrite(motor->pin1, LOW);
    digitalWrite(motor->pin2, LOW);
    analogWrite(motor->enable, 0);
}

// Convert PWM values to flow rate, depending on motor type and direction
float conv_flow_rate(byte dutyCycle, byte flag, byte id)
{

    if (id == 1)
    {
        if (flag == 2)
        {
            return get_flow_rate(dutyCycle, &revB);
        }
        else
        {
            return get_flow_rate(dutyCycle, &fwdB);
        }
    }

    else
    {
        if (flag == 2)
        {
            return get_flow_rate(dutyCycle, &revA);
        }
        else
        {
            return get_flow_rate(dutyCycle, &fwdB);
        }
    }
}

// Converts dutyCycle to calibrated flow rate
float get_flow_rate(byte dutyCycle, Calib *calib)
{
    // Third order polynomial.
    // Table lookup would be more efficient, but not needed

    float flow_rate = ((calib->a) * dutyCycle * dutyCycle * dutyCycle) + ((calib->b) * dutyCycle * dutyCycle) + ((calib->c) * dutyCycle) + (calib->d);
    return flow_rate;
}

// Reads control potentiometer and generates duty cycle and flow rate
void control_flow(Assem *assem)
{
    // Read the potentiometer
    assem->flow->potVal = analogRead(assem->control->pot);

    // Map analogue read to from motor.minPWM to motor.maxPWM lower than this stalls the motor
    assem->flow->dutyCycle = map(assem->flow->potVal, 0, 1023, assem->motor->minPWM, assem->motor->maxPWM);

    // Calculate the calibrated flow rate
    assem->flow->flowRate = conv_flow_rate(assem->flow->dutyCycle, assem->control->flag, assem->motor->id);
}