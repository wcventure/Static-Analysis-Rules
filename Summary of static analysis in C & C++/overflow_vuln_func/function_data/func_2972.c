static int fifo_put(SerialState *s, int fifo, uint8_t chr)
{
    SerialFIFO *f = (fifo) ? &s->recv_fifo : &s->xmit_fifo;

    f->data[f->head++] = chr;

    if (f->head == UART_FIFO_LENGTH)
        f->head = 0;
    f->count++;

    return 1;
}
