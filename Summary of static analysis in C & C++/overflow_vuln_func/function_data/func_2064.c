static void pmac_ide_atapi_transfer_cb(void *opaque, int ret)
{
    DBDMA_io *io = opaque;
    MACIOIDEState *m = io->opaque;
    IDEState *s = idebus_active_if(&m->bus);
    int64_t offset;

    MACIO_DPRINTF("pmac_ide_atapi_transfer_cb\n");

    if (ret < 0) {
        MACIO_DPRINTF("DMA error: %d\n", ret);
        ide_atapi_io_error(s, ret);
        goto done;
    }

    if (!m->dma_active) {
        MACIO_DPRINTF("waiting for data (%#x - %#x - %x)\n",
                      s->nsector, io->len, s->status);
        /
        io->processing = false;
        return;
    }

    if (s->io_buffer_size <= 0) {
        MACIO_DPRINTF("End of IDE transfer\n");
        ide_atapi_cmd_ok(s);
        m->dma_active = false;
        goto done;
    }

    if (io->len == 0) {
        MACIO_DPRINTF("End of DMA transfer\n");
        goto done;
    }

    if (s->lba == -1) {
        /
        s->io_buffer_size = MIN(s->io_buffer_size, io->len);
        cpu_physical_memory_write(io->addr, s->io_buffer, s->io_buffer_size);
        ide_atapi_cmd_ok(s);
        m->dma_active = false;
        goto done;
    }

    /
    offset = (int64_t)(s->lba << 11) + s->io_buffer_index;

    pmac_dma_read(s->blk, offset, io->len, pmac_ide_atapi_transfer_cb, io);
    return;

done:
    if (ret < 0) {
        block_acct_failed(blk_get_stats(s->blk), &s->acct);
    } else {
        block_acct_done(blk_get_stats(s->blk), &s->acct);
    }
    io->dma_end(opaque);
}
