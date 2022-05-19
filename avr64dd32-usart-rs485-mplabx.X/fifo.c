#include "fifo.h"

void FIFO_init(FIFO_t *pFifo)
{
    pFifo->rear = 0;
    pFifo->front = 0;
    pFifo->isFull = false;
    pFifo->isEmpty = true;
}

FIFO_status_t FIFO_get(FIFO_t *pFifo, uint8_t *data)
{
    if (pFifo->isEmpty == true)
    {
        return FIFO_ERR;
    }

    pFifo->isFull = false;
    *data = pFifo->buffer[pFifo->front++];

    if (pFifo->front == FIFO_BUFFER_SIZE)
    {
        pFifo->front = 0;
    }

    if (pFifo->front == pFifo->rear)
    {
        pFifo->isEmpty = true;
    }

    return FIFO_OK;
}

FIFO_status_t FIFO_put(FIFO_t *pFifo, uint8_t data)
{
    if (pFifo->isFull == true)
    {
        return FIFO_ERR;
    }

    pFifo->isEmpty = false;
    pFifo->buffer[pFifo->rear++] = data;

    if (pFifo->rear == FIFO_BUFFER_SIZE)
    {
        pFifo->rear = 0;
    }

    if (pFifo->rear == pFifo->front)
    {
        pFifo->isFull = true;
    }

    return FIFO_OK;
}
