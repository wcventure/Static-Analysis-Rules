static bool scsi_target_emulate_inquiry(SCSITargetReq *r)
{
    assert(r->req.dev->lun != r->req.lun);
    if (r->req.cmd.buf[1] & 0x2) {
        /
        return false;
    }

    if (r->req.cmd.buf[1] & 0x1) {
        /
        uint8_t page_code = r->req.cmd.buf[2];
        r->buf[r->len++] = page_code ; /
        r->buf[r->len++] = 0x00;

        switch (page_code) {
        case 0x00: /
        {
            int pages;
            pages = r->len++;
            r->buf[r->len++] = 0x00; /
            r->buf[pages] = r->len - pages - 1; /
            break;
        }
        default:
            return false;
        }
        /
        assert(r->len < sizeof(r->buf));
        r->len = MIN(r->req.cmd.xfer, r->len);
        return true;
    }

    /
    if (r->req.cmd.buf[2] != 0) {
        return false;
    }

    /
    r->len = MIN(r->req.cmd.xfer, 36);
    memset(r->buf, 0, r->len);
    if (r->req.lun != 0) {
        r->buf[0] = TYPE_NO_LUN;
    } else {
        r->buf[0] = TYPE_NOT_PRESENT | TYPE_INACTIVE;
        r->buf[2] = 5; /
        r->buf[3] = 2 | 0x10; /
        r->buf[4] = r->len - 5; /
        r->buf[7] = 0x10 | (r->req.bus->info->tcq ? 0x02 : 0); /
        memcpy(&r->buf[8], "QEMU    ", 8);
        memcpy(&r->buf[16], "QEMU TARGET     ", 16);
        pstrcpy((char *) &r->buf[32], 4, qemu_get_version());
    }
    return true;
}
