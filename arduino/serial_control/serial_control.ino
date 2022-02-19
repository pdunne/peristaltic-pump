/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * Copyright 2022 Peter Dunne
 */

/*
Peristaltic Pump Control

Serial control of two DC pumps simultaneously.
Output the rotation direction and rotation rate as calibrated flow rate on a
128x64 OLED

Updated Feb 2022
By Peter Dunne

https://www.applied-magnetism.com/
https://github.com/pdunne/peristaltic-pump
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)

/* New way to define screen properties with SSD1306 library. Might need to be
commented out for older versions of header. */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int loop_delay = 100; // Loop delay in ms

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

typedef struct Motor Motor;
typedef struct Control Control;
typedef struct Flow Flow;

// Motor connections on H-Bridge
Motor motA = {11, 12, 13, 170, 255, 0};
Motor motB = {5, 4, 2, 180, 255, 1};

// Direction control pins
Control controlA = {7, 8, 0, 0};
Control controlB = {0, 1, 1, 0};

Flow flowA = {0, 0, 0.0};
Flow flowB = {0, 0, 0.0};

/******************************************/

// Function Prototypes
void setup(void);
void loop(void);
void init_switch(Control *control);
void init_motor(const Motor *motor);
void off_motor(const Motor *motor);
void motor_stop(const Motor *motor);
void motor_forward(const Motor *motor, byte dutyCycle);
void motor_reverse(const Motor *motor, byte dutyCycle);
void motor_check(const Motor *motor, Control *control, byte dutyCycle);
void init_display(byte dutyCycle);
void print_flow_rate(float flow_rate);
float conv_flow_rate(byte dutyCycle);
void control_flow(Motor *motor, Control *control, Flow *flow);

/******************************************/

// Setup board
void setup()
{
  // Set all the motor control pins to outputs
  init_motor(&motA);
  init_motor(&motB);

  // Turn off motor - Initial state
  off_motor(&motA);
  off_motor(&motB);

  // Pins for Detecting Forward or Reverse
  init_switch(&controlA);
  init_switch(&controlB);

  // Serial.begin(9600);
  // Set the address of the OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Clear the buffer.
  display.clearDisplay();

  // Display Text
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.display();
  display.clearDisplay();
}
/******************************************/

// Detection of pins and control of pumps
void loop()
{
  // Reads potentiometer and sets the flow rate
  control_flow(&motA, &controlA, &flowA);
  control_flow(&motB, &controlB, &flowB);

  // Initialise display
  init_display(flowA.dutyCycle);

  // Check motor and display status
  display.setCursor(0, 40);

  motor_check(&motA, &controlA, flowA.dutyCycle);
  // display.setCursor(20, 40);
  motor_check(&motB, &controlB, flowB.dutyCycle);

  // Display Flow rate
  // print_flow_rate(flowA.flowRate);
  // print_flow_rate(flowB.flowRate);

  display.display();
  delay(loop_delay);
}

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

// Turns off a motor
void motor_stop(const Motor *motor)
{
  display.println("Off");
  off_motor(motor);
}

// Drives the motor forward at duty cycle 'dutyCycle'
void motor_forward(const Motor *motor, byte dutyCycle)
{
  display.println("Forward");
  digitalWrite(motor->pin1, HIGH);
  digitalWrite(motor->pin2, LOW);
  analogWrite(motor->enable, dutyCycle);
}

// Drives the motor in reverse at duty cycle 'dutyCycle'
void motor_reverse(const Motor *motor, byte dutyCycle)
{
  display.println("Reverse");
  digitalWrite(motor->pin1, LOW);
  digitalWrite(motor->pin2, HIGH);
  analogWrite(motor->enable, dutyCycle);
}

// Logic to check motor switch state
void motor_check(const Motor *motor, Control *control, byte dutyCycle)
{

  if (digitalRead(control->fwd) == LOW)
  {
    control->flag = 1;
    motor_forward(motor, dutyCycle);
  }
  else if (digitalRead(control->rev) == LOW)
  {
    control->flag = 2;
    motor_reverse(motor, dutyCycle);
  }
  else
  {
    control->flag = 0;
    motor_stop(motor);
  }
}

// Initialise display
void init_display(byte dutyCycle)
{
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("Motor:");
  display.setCursor(64, 0);
  display.println("Flow Rate:");
  display.setTextSize(2);
  // display.print(map(dutyCycle, min_PWM, max_PWM, 0, 100), DEC);
  display.print(flowA.dutyCycle);
  display.print(flowB.dutyCycle);

  // display.println("%");
}

// Display flow rate on OLED
void print_flow_rate(float flow_rate)
{
  display.setCursor(64, 8);
  display.println(flow_rate, 1);
  display.setCursor(64, 24);
  display.print("ml/mn");
}

// Convert PWM values to flow rate. Only valid for forward flow, not reverse.
float conv_flow_rate(byte dutyCycle)
{
  // const float slope = 0.8073;
  // const float offset = -138.688;
  // float flow_rate = dutyCycle * slope + offset;

  // Third order polynomial.
  // Table lookup would be more efficient, but not needed
  const float a = -1.70373460e-04;
  const float b = 1.19023534e-01;
  const float c = -2.68600744e+01;
  const float d = 2.00133906e+03;
  float flow_rate = a * dutyCycle * dutyCycle * dutyCycle + b * dutyCycle * dutyCycle + c * dutyCycle + d;
  return flow_rate;
}

void control_flow(Motor *motor, Control *control, Flow *flow)
{
  // Read the potentiometer
  flow->potVal = analogRead(control->pot);

  // Map analogue read to from motor.minPWM to motor.maxPWM lower than this stalls the motor
  flow->dutyCycle = map(flow->potVal, 0, 1023, motor->minPWM, motor->maxPWM);

  // Calculate the calibrated flow rate
  flow->flowRate = conv_flow_rate(flow->dutyCycle);
}
/*******************************************/
