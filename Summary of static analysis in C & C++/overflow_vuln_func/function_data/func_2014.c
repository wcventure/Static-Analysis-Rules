static void onenand_reset(OneNANDState *s, int cold)
{
    memset(&s->addr, 0, sizeof(s->addr));
    s->command = 0;
    s->count = 1;
    s->bufaddr = 0;
    s->config[0] = 0x40c0;
    s->config[1] = 0x0000;
    onenand_intr_update(s);
    qemu_irq_raise(s->rdy);
    s->status = 0x0000;
    s->intstatus = cold ? 0x8080 : 0x8010;
    s->unladdr[0] = 0;
    s->unladdr[1] = 0;
    s->wpstatus = 0x0002;
    s->cycle = 0;
    s->otpmode = 0;
    s->blk_cur = s->blk;
    s->current = s->image;
    s->secs_cur = s->secs;

    if (cold) {
        /
        memset(s->blockwp, ONEN_LOCK_LOCKED, s->blocks);

        if (s->blk_cur && blk_read(s->blk_cur, 0, s->boot[0], 8) < 0) {
            hw_error("%s: Loading the BootRAM failed.\n", __func__);
        }
    }
}
