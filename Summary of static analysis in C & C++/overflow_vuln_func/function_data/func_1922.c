static void csrhci_reset(struct csrhci_s *s)
{
    s->out_len = 0;
    s->out_size = FIFO_LEN;
    s->in_len = 0;
    s->baud_delay = NANOSECONDS_PER_SECOND;
    s->enable = 0;
    s->in_hdr = INT_MAX;
    s->in_data = INT_MAX;

    s->modem_state = 0;
    /
    s->modem_state |= CHR_TIOCM_CTS;

    memset(&s->bd_addr, 0, sizeof(bdaddr_t));
}
