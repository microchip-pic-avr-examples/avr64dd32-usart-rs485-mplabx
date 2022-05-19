#include <stdlib.h>
#include "serial.h"
#include "rs485.h"
#include "fifo.h"
#include "bridge.h"

static BRIDGE_overflow_cb_t pRs485_overflow_cb = NULL;
static BRIDGE_overflow_cb_t pSerial_overflow_cb = NULL;

static FIFO_t BRIDGE_serialToRs485Queue;
static FIFO_t BRIDGE_rs485ToSerialQueue;

static inline void BRIDGE_rs485_overflowCb(bool status)
{
    if (pRs485_overflow_cb != NULL)
    {
        pRs485_overflow_cb(status);
    }
}

static inline void BRIDGE_serial_overflowCb(bool status)
{
    if (pSerial_overflow_cb != NULL)
    {
        pSerial_overflow_cb(status);
    }
}

static void BRIDGE_serialRx(uint8_t byte)
{

    if (FIFO_put(&BRIDGE_serialToRs485Queue, byte) == FIFO_ERR)
    {
        BRIDGE_serial_overflowCb(true);
    }

    BRIDGE_serial_overflowCb(false);
    RS485_controlTx(true);
}

static bool BRIDGE_serialTx(uint8_t *pByte)
{
    if (FIFO_get(&BRIDGE_rs485ToSerialQueue, pByte) == FIFO_ERR)
    {
        SERIAL_controlTx(false);
        return false;
    }

    return true;
}

static void BRIDGE_rs485Rx(uint8_t byte)
{
    if (FIFO_put(&BRIDGE_rs485ToSerialQueue, byte) == FIFO_ERR)
    {
        BRIDGE_rs485_overflowCb(true);
    }

    BRIDGE_rs485_overflowCb(false);
    SERIAL_controlTx(true);
}

static bool BRIDGE_rs485Tx(uint8_t *pByte)
{
    if (FIFO_get(&BRIDGE_serialToRs485Queue, pByte) == FIFO_ERR)
    {
        RS485_controlTx(false);
        return false;
    }

    return true;
}

void BRIDGE_init(void)
{
    pRs485_overflow_cb = NULL;
    pSerial_overflow_cb = NULL;

    SERIAL_init();
    SERIAL_setRecvCb(BRIDGE_serialRx);
    SERIAL_setSendCb(BRIDGE_serialTx);

    RS485_init();
    RS485_setRecvCb(BRIDGE_rs485Rx);
    RS485_setSendCb(BRIDGE_rs485Tx);

    FIFO_init(&BRIDGE_serialToRs485Queue);
    FIFO_init(&BRIDGE_rs485ToSerialQueue);
}

void BRIDGE_setRs485_ovfCb(BRIDGE_overflow_cb_t pCbFunc)
{
    pRs485_overflow_cb = pCbFunc;
}

void BRIDGE_setSerial_ovfCb(BRIDGE_overflow_cb_t pCbFunc)
{
    pSerial_overflow_cb = pCbFunc;
}
