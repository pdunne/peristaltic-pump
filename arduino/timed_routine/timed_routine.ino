#include "src/peripump.h"

// Motor connections on H - Bridge : 11, 12, 13, PWM 170 - 255, ID 0
const Motor motA = {11, 12, 13, 170, 255, 0};
// Direction control pins: fwd 7, rev 8, pot pin A0, flag 0
Control controlA = {7, 8, 0, 0};
// Potentiometer val 0, duty cycle 0%, flow rate 0 ml/min
Flow flowA = {0, 0, 0.0};

// Motor connections on H - Bridge : 5, 4, 123, PWM 180 - 255, ID 1
const Motor motB = {5, 4, 2, 180, 255, 1};
// Direction control pins: fwd 0, rev 1, pot pin A1, flag 0
Control controlB = {0, 1, 1, 0};
// Potentiometer val 0, duty cycle 0%, flow rate 0 ml/min
Flow flowB = {0, 0, 0.0};

// Collect the motor structs
Assem assemA = {&motA, &controlA, &flowA};
Assem assemB = {&motB, &controlB, &flowB};

const int loop_delay = 100; // Loop delay in ms

unsigned long end_time;     // For timed loops
unsigned long current_time; // Current time
unsigned long loop_time;    // Start time of loop

Screen display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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

    current_time = millis();
    end_time = current_time + (60 * 1000L);
}

void loop()
{
    if (millis() < end_time)
    {

        // Reads potentiometer and sets the flow rate
        control_flow(&assemA);
        control_flow(&assemB);

        // Initialise display
        init_display(&display, &assemA, &assemB);

        // Check motor and display status
        display.setCursor(0, 40);
        motor_check(&display, &assemA);
        motor_check(&display, &assemB);

        // Display Flow rate
        print_pwm(&display, &assemA);
        print_pwm(&display, &assemB);

        display.setCursor(0, 48);
        current_time = millis();
        display.print((end_time - current_time) / 1000L, DEC);

        display.print(" s");

        display.display();
        delay(loop_delay);
    }
    else
    {
        motor_stop(&display, &motA);
        motor_stop(&display, &motB);
    }
}
