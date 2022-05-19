#include "led.h"
#include <avr/io.h>

void LED_init(void)
{
    PORTF.OUTSET = PIN5_bm;
    PORTF.DIRSET = PIN5_bm;
}

void LED_toggle(void)
{
    PORTF.OUTTGL = PIN5_bm;
}

void LED_turnOn(void)
{
    PORTF.OUTCLR = PIN5_bm;
}

void LED_turnOff(void)
{
    PORTF.OUTSET = PIN5_bm;
}
