/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * Copyright 2021 Peter Dunne
 */

/*
  Peristaltic Pump Control

	* Use a toggle switch to change between off, forward, and reverse flow
  * A potentiometer to vary the rotation rate of the DC motor.
  * Output the rotation direction and rotation rate as calibrated flow rate on a
  128x64 OLED


	The circuit:
	* Toggle Switch on pins 7, 8 for detecting rotation direction
	* list the components attached to each output

  L293D DC motor drive


	Created October 2020
	By Peter Dunne
	

	https://www.applied-magnetism.com/
  https://github.com/pdunne/peristaltic-pump
*/

/******************************************/

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

const byte pot_read_pin = 0; // Input pin for reading potentiometer
int pot_value = 0;           // Potentiometer value
byte duty_cycle = 0;         // Mapped duty cycle
const int loop_delay = 100;  // Loop delay in ms

float flow_rate = 0.0; // Flow rate in l/min

// Motor A connections on H-Bridge
const byte enable_motor_A = 11; // Enable motor A
const byte in1 = 12;            // Input 1
const byte in2 = 13;            // Input 2

// Switch pins
const byte fwd_pin = 7; // Forward
const byte rev_pin = 8; // Reverse
/******************************************/

void setup()
{
  // Set all the motor control pins to outputs
  pinMode(enable_motor_A, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  digitalWrite(enable_motor_A, LOW);

  // Turn off motor - Initial state
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  // Pins for Detecting Forward or Reverse
  pinMode(fwd_pin, INPUT_PULLUP);
  pinMode(rev_pin, INPUT_PULLUP);

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
void loop()
{
  // Read the potentiometer
  pot_value = analogRead(pot_read_pin);

  // Map analogue read to 210-255 lower than this stalls the motor
  duty_cycle = map(pot_value, 0, 1023, 210, 255);

  // Calculate the calibrated flow rate
  flow_rate = conv_flow_rate(duty_cycle);

  // Initialise display
  init_display(duty_cycle);

  // Check motor and display status
  motor_check();

  // Display Flow rate
  print_flow_rate(flow_rate);

  display.display();
  delay(loop_delay);
}

// Logic to check motor switch state
void motor_check()
{
  display.setCursor(0, 40);

  if (digitalRead(fwd_pin) == LOW)
  {
    motor_forward(duty_cycle);
  }
  else if (digitalRead(rev_pin) == LOW)
  {
    motor_reverse(duty_cycle);
  }
  else
  {
    motor_stop();
  }
}

// Drives the motor forward at duty cycle 'duty_cycle'
void motor_forward(byte duty_cycle)
{
  display.println("Forward");
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enable_motor_A, duty_cycle);
}

// Drives the motor in reverse at duty cycle 'duty_cycle'
void motor_reverse(byte duty_cycle)
{
  display.println("Reverse");
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enable_motor_A, duty_cycle);
}

// Turns off the motor
void motor_stop()
{
  display.println("Off");
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enable_motor_A, 0);
}

// Initialise display
void init_display(byte duty_cycle)
{
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("Motor:");
  display.setCursor(64, 0);
  display.println("Flow Rate:");
  display.setTextSize(2);
  display.print(map(duty_cycle, 210, 255, 0, 100), DEC);
  display.println("%");
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
float conv_flow_rate(byte duty_cycle)
{
  // const float slope = 0.8073;
  // const float offset = -138.688;
  // float flow_rate = duty_cycle * slope + offset;

  // Third order polynomial.
  // Table lookup would be more efficient, but not needed
  const float a = -1.70373460e-04;
  const float b = 1.19023534e-01;
  const float c = -2.68600744e+01;
  const float d = 2.00133906e+03;
  float flow_rate = a * duty_cycle * duty_cycle * duty_cycle + b * duty_cycle * duty_cycle + c * duty_cycle + d;
  return flow_rate;
}

/*******************************************/
