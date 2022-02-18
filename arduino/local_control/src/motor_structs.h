#ifndef MOTOR_STRUCTS_H
#define MOTOR_STRUCTS_H

// #include <Arduino.h>

// Defines control pins, and min and max PWM values for a DC motor
struct Motor
{
    byte enable; // Enable motor (Digital pin)
    byte pin1;   // Input 1 (Digital pin)
    byte pin2;   // Input 2 (Digital pin)
    byte minPWM; // Minimum PWM duty cycle
    byte maxPWM; // Maximum PWM duty cycle
    byte id;     // Pump ID
};

// Defines forward and reverse detection pins
struct Control
{
    const byte fwd; // Forward Pin (Digital)
    const byte rev; // Reverse Pin (Digital)
    const byte pot; // Potentiometer Pin (Analog)
    byte flag;      // Status flag 0 - off, 1 - forward, 2 - reverse
};

// Stores potentiometer reading, duty cycle %, and calibrated flow rate in l/min
struct Flow
{
    int potVal;     // Potentiometer value
    byte dutyCycle; // Mapped duty cycle
    float flowRate; // Flow rate in l/min
};

// Stores pointers to Motor, Control, and Flow structs
struct Assem
{
    const Motor *motor;
    Control *control;
    Flow *flow;
};

typedef struct Motor Motor;
typedef struct Control Control;
typedef struct Flow Flow;
typedef struct Assem Assem;

#endif