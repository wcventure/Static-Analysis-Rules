void qemu_bh_delete(QEMUBH *bh)
{
    qemu_bh_cancel(bh);
    qemu_free(bh);
}
