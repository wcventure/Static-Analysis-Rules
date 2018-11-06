static void complete_collecting_data(Flash *s)
{
    int i;

    s->cur_addr = 0;

    for (i = 0; i < get_addr_length(s); ++i) {
        s->cur_addr <<= 8;
        s->cur_addr |= s->data[i];
    }

    if (get_addr_length(s) == 3) {
        s->cur_addr += s->ear * MAX_3BYTES_SIZE;
    }

    s->state = STATE_IDLE;

    switch (s->cmd_in_progress) {
    case DPP:
    case QPP:
    case PP:
    case PP4:
    case PP4_4:
        s->state = STATE_PAGE_PROGRAM;
        break;
    case READ:
    case READ4:
    case FAST_READ:
    case FAST_READ4:
    case DOR:
    case DOR4:
    case QOR:
    case QOR4:
    case DIOR:
    case DIOR4:
    case QIOR:
    case QIOR4:
        s->state = STATE_READ;
        break;
    case ERASE_4K:
    case ERASE4_4K:
    case ERASE_32K:
    case ERASE4_32K:
    case ERASE_SECTOR:
    case ERASE4_SECTOR:
        flash_erase(s, s->cur_addr, s->cmd_in_progress);
        break;
    case WRSR:
        switch (get_man(s)) {
        case MAN_SPANSION:
            s->quad_enable = !!(s->data[1] & 0x02);
            break;
        case MAN_MACRONIX:
            s->quad_enable = extract32(s->data[0], 6, 1);
            if (s->len > 1) {
                s->four_bytes_address_mode = extract32(s->data[1], 5, 1);
            }
            break;
        default:
            break;
        }
        if (s->write_enable) {
            s->write_enable = false;
        }
        break;
    case EXTEND_ADDR_WRITE:
        s->ear = s->data[0];
        break;
    case WNVCR:
        s->nonvolatile_cfg = s->data[0] | (s->data[1] << 8);
        break;
    case WVCR:
        s->volatile_cfg = s->data[0];
        break;
    case WEVCR:
        s->enh_volatile_cfg = s->data[0];
        break;
    default:
        break;
    }
}
