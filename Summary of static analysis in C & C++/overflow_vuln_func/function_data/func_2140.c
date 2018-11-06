static uint32_t m25p80_transfer8(SSISlave *ss, uint32_t tx)
{
    Flash *s = M25P80(ss);
    uint32_t r = 0;

    switch (s->state) {

    case STATE_PAGE_PROGRAM:
        DB_PRINT_L(1, "page program cur_addr=%#" PRIx64 " data=%" PRIx8 "\n",
                   s->cur_addr, (uint8_t)tx);
        flash_write8(s, s->cur_addr, (uint8_t)tx);
        s->cur_addr++;
        break;

    case STATE_READ:
        r = s->storage[s->cur_addr];
        DB_PRINT_L(1, "READ 0x%" PRIx64 "=%" PRIx8 "\n", s->cur_addr,
                   (uint8_t)r);
        s->cur_addr = (s->cur_addr + 1) % s->size;
        break;

    case STATE_COLLECTING_DATA:
    case STATE_COLLECTING_VAR_LEN_DATA:
        s->data[s->len] = (uint8_t)tx;
        s->len++;

        if (s->len == s->needed_bytes) {
            complete_collecting_data(s);
        }
        break;

    case STATE_READING_DATA:
        r = s->data[s->pos];
        s->pos++;
        if (s->pos == s->len) {
            s->pos = 0;
            s->state = STATE_IDLE;
        }
        break;

    default:
    case STATE_IDLE:
        decode_new_cmd(s, (uint8_t)tx);
        break;
    }

    return r;
}
