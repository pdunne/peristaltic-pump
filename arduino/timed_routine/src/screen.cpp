#include "screen.h"

// Initialise display
void init_display(Screen *display, Assem *assemA, Assem *assemB)
{
    (*display).clearDisplay();
    (*display).setTextColor(WHITE);
    (*display).setCursor(0, 0);
    (*display).setTextSize(1);
    (*display).print("DC %");
    (*display).setCursor(44, 0);
    (*display).print("Dir");
    (*display).setCursor(70, 0);
    (*display).println("ml/min");
    (*display).setTextSize(2);

    print_flow_range(display, assemA);
    print_flow_range(display, assemB);
}

// Prints Duty Cycle Percentage on OLED
void print_flow_range(Screen *display, Assem *assem)
{
    if (assem->motor->id == 1)
    {
        (*display).setCursor(0, 8);
    }
    else
    {
        (*display).setCursor(0, 24);
    }
    (*display).print(map(assem->flow->dutyCycle, assem->motor->minPWM, assem->motor->maxPWM, 0, 100), DEC);
}

// Prints flow rate on OLED
void print_flow_rate(Screen *display, Assem *assem)
{
    if (assem->motor->id == 1)
    {
        (*display).setCursor(72, 8);
    }
    else
    {
        (*display).setCursor(72, 24);
    }

    (*display).println(assem->flow->flowRate, 1);
}
