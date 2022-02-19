#ifndef SCREEN_H
#define SCREEN_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "motor_structs.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)

typedef Adafruit_SSD1306 Screen;
/* New way to define screen properties with SSD1306 library. Might need to be
commented out for older versions of header. */

void init_display(Screen *display, Assem *assemA, Assem *assemB);
void print_flow_rate(Screen *display, Assem *assem);
void print_flow_range(Screen *display, Assem *assem);
#endif