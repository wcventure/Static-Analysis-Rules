static uint64_t uart_read(void *opaque, target_phys_addr_t offset,
        unsigned size)
{
    UartState *s = (UartState *)opaque;
    uint32_t c = 0;

    offset >>= 2;
    if (offset > R_MAX) {
        return 0;
    } else if (offset == R_TX_RX) {
        uart_read_rx_fifo(s, &c);
        return c;
    }
    return s->r[offset];
}
