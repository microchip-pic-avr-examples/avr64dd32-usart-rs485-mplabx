#include "rs485.h"
#include "utils.h"
#include <stdlib.h>
#include <avr/interrupt.h>

#define RS485_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)(BAUD_RATE))) + 0.5)

static RS485_rx_cb_t pRxCb = NULL;
static RS485_tx_cb_t pTxCb = NULL;

static void RS485_evsysInit(void)
{
    EVSYS.CHANNEL2 = EVSYS_CHANNEL2_PORTC_PIN3_gc;
    EVSYS.USEREVSYSEVOUTC = EVSYS_USER_CHANNEL2_gc; 
}

static void RS485_portInit(void)
{
    PORTC.OUTSET = PIN0_bm; /* TX */
    PORTC.DIRSET = PIN0_bm; 

    PORTC.DIRCLR = PIN1_bm; /* RX */

    PORTC.DIRSET = PIN3_bm; /* XDIR */

    PORTC.DIRSET = PIN2_bm; /* Data Enable Pin */
}

static void RS485_usartInit(void)
{
    USART1.BAUD = (uint16_t)RS485_BAUD_RATE(9600);

    USART1.CTRLA = 1 << USART_RXCIE_bp | USART_RS485_ENABLE_gc;

    USART1.CTRLB = 1 << USART_RXEN_bp | 1 << USART_TXEN_bp | USART_RXMODE_NORMAL_gc;
}

void RS485_init(void)
{
    pTxCb = NULL;
    pRxCb = NULL;
    RS485_portInit();
    RS485_usartInit();
    RS485_evsysInit();
}

void RS485_controlTx(bool txControl)
{
    if (txControl)
    {
        USART1.CTRLA |= (1 << USART_DREIE_bp);
    }
    else
    {
        USART1.CTRLA &= ~(1 << USART_DREIE_bp);
    }
}

void RS485_setRecvCb(RS485_rx_cb_t pCbFunc)
{
    pRxCb = pCbFunc;
}

void RS485_setSendCb(RS485_tx_cb_t pCbFunc)
{
    pTxCb = pCbFunc;
}

ISR(USART1_RXC_vect)
{
    if (pRxCb != NULL)
    {
        pRxCb(USART1.RXDATAL);
    }
}

ISR(USART1_DRE_vect)
{
    uint8_t byte = 0;

    if (pTxCb != NULL)
    {
        if (pTxCb(&byte) == true)
        {
            USART1.TXDATAL = byte;
        }
    }
}
