#include "serial.h"
#include "utils.h"
#include <stdlib.h>
#include <avr/interrupt.h>

#define SERIAL_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)(BAUD_RATE))) + 0.5)

static SERIAL_rx_cb_t pRxCb = NULL;
static SERIAL_tx_cb_t pTxCb = NULL;

static void SERIAL_portInit(void)
{
    PORTD.OUTSET = PIN4_bm; /* TX */
    PORTD.DIRSET = PIN4_bm; 

    PORTD.DIR &= ~PIN5_bm; /* RX */
}

static void SERIAL_portmuxInit(void)
{
    PORTMUX.USARTROUTEA = PORTMUX_USART0_ALT3_gc;   /* USART0 routed to PORTD that is connected to CDC */
}

static void SERIAL_usartInit(void)
{
    USART0.BAUD = (uint16_t)SERIAL_BAUD_RATE(115200);

    USART0.CTRLA = 1 << USART_RXCIE_bp | USART_RS485_DISABLE_gc;

    USART0.CTRLB = 1 << USART_RXEN_bp | 1 << USART_TXEN_bp | USART_RXMODE_NORMAL_gc;
}

void SERIAL_init(void)
{
    pTxCb = NULL;
    pRxCb = NULL;
    SERIAL_portInit();
    SERIAL_portmuxInit();
    SERIAL_usartInit();
}

void SERIAL_controlTx(bool txControl)
{
    if (txControl)
    {
        USART0.CTRLA |= (1 << USART_DREIE_bp);
    }
    else
    {
        USART0.CTRLA &= ~(1 << USART_DREIE_bp);
    }
}

void SERIAL_setRecvCb(SERIAL_rx_cb_t pCbFunc)
{
    pRxCb = pCbFunc;
}

void SERIAL_setSendCb(SERIAL_tx_cb_t pCbFunc)
{
    pTxCb = pCbFunc;
}

ISR(USART0_RXC_vect)
{
    if (pRxCb != NULL)
    {
        pRxCb(USART0.RXDATAL);
    }
}

ISR(USART0_DRE_vect)
{
    uint8_t byte = 0;

    if (pTxCb != NULL)
    {
        if (pTxCb(&byte) == true)
        {
            USART0.TXDATAL = byte;
        }
    }
}
