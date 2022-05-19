#include <stdbool.h>
#include <avr/interrupt.h>
#include "led.h"
#include "bridge.h"


/* Default fuses configuration:
- BOD disabled
- Oscillator in High-Frequency Mode
- UPDI pin active(WARNING: DO NOT CHANGE!)
- RESET pin used as GPIO
- CRC disabled
- MVIO enabled for dual supply
- Watchdog Timer disabled
*/
FUSES =
{
.BODCFG = ACTIVE_DISABLE_gc | LVL_BODLEVEL0_gc | SAMPFREQ_128Hz_gc | SLEEP_DISABLE_gc,
.BOOTSIZE = 0x0,
.CODESIZE = 0x0,
.OSCCFG = CLKSEL_OSCHF_gc,
.SYSCFG0 = CRCSEL_CRC16_gc | CRCSRC_NOCRC_gc | RSTPINCFG_GPIO_gc | UPDIPINCFG_UPDI_gc,
.SYSCFG1 = MVSYSCFG_DUAL_gc | SUT_0MS_gc,
.WDTCFG = PERIOD_OFF_gc | WINDOW_OFF_gc,
};

void setLED(bool level);

int main(void)
{
    LED_init();

    BRIDGE_init();
    BRIDGE_setSerial_ovfCb(setLED);
    BRIDGE_setRs485_ovfCb(setLED);

    sei();

    while (true)
    {
        ;
    }

    return 0;
}

void setLED(bool level)
{
    if (level == true)
    {
        LED_turnOn();
    }
    else
    {
        LED_turnOff();
    }
}