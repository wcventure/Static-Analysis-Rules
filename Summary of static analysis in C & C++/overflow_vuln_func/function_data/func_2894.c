void qemu_bh_cancel(QEMUBH *bh)
{
    QEMUBH **pbh;
    if (bh->scheduled) {
        pbh = &first_bh;
        while (*pbh != bh)
            pbh = &(*pbh)->next;
        *pbh = bh->next;
        bh->scheduled = 0;
    }
}
