int qemu_bh_poll(void)
{
    QEMUBH *bh, **pbh;
    int ret;

    ret = 0;
    for(;;) {
        pbh = &first_bh;
        bh = *pbh;
        if (!bh)
            break;
        ret = 1;
        *pbh = bh->next;
        bh->scheduled = 0;
        bh->cb(bh->opaque);
    }
    return ret;
}
