static void continue_send(IPMIBmcExtern *ibe)
{
    if (ibe->outlen == 0) {
        goto check_reset;
    }
 send:
    ibe->outpos += qemu_chr_fe_write(ibe->chr, ibe->outbuf + ibe->outpos,
                                     ibe->outlen - ibe->outpos);
    if (ibe->outpos < ibe->outlen) {
        /
        timer_mod_ns(ibe->extern_timer,
                     qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + 10000000);
    } else {
        /
        ibe->outlen = 0;
        ibe->outpos = 0;
        if (!ibe->sending_cmd) {
            ibe->waiting_rsp = true;
        } else {
            ibe->sending_cmd = false;
        }
    check_reset:
        if (ibe->connected && ibe->send_reset) {
            /
            ibe->outbuf[0] = VM_CMD_RESET;
            ibe->outbuf[1] = VM_CMD_CHAR;
            ibe->outlen = 2;
            ibe->outpos = 0;
            ibe->send_reset = false;
            ibe->sending_cmd = true;
            goto send;
        }

        if (ibe->waiting_rsp) {
            /
            timer_mod_ns(ibe->extern_timer,
                         qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + 4000000000ULL);
        }
    }
    return;
}
