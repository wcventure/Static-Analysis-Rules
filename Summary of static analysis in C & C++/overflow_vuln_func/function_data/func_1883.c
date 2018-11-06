void ide_dma_cb(void *opaque, int ret)
{
    IDEState *s = opaque;
    int n;
    int64_t sector_num;

    if (ret < 0) {
        int op = BM_STATUS_DMA_RETRY;

        if (s->is_read)
            op |= BM_STATUS_RETRY_READ;
        if (ide_handle_rw_error(s, -ret, op)) {
            return;
        }
    }

    n = s->io_buffer_size >> 9;
    sector_num = ide_get_sector(s);
    if (n > 0) {
        dma_buf_commit(s, s->is_read);
        sector_num += n;
        ide_set_sector(s, sector_num);
        s->nsector -= n;
    }

    /
    if (s->nsector == 0) {
        s->status = READY_STAT | SEEK_STAT;
        ide_set_irq(s->bus);
        goto eot;
    }

    /
    n = s->nsector;
    s->io_buffer_index = 0;
    s->io_buffer_size = n * 512;
    if (s->bus->dma->ops->prepare_buf(s->bus->dma, s->is_read) == 0)
        goto eot;

#ifdef DEBUG_AIO
    printf("ide_dma_cb: sector_num=%" PRId64 " n=%d, is_read=%d\n",
           sector_num, n, s->is_read);
#endif

    if (s->is_read) {
        s->bus->dma->aiocb = dma_bdrv_read(s->bs, &s->sg, sector_num,
                                           ide_dma_cb, s);
    } else {
        s->bus->dma->aiocb = dma_bdrv_write(s->bs, &s->sg, sector_num,
                                            ide_dma_cb, s);
    }
    ide_dma_submit_check(s, ide_dma_cb);
    return;

eot:
   s->bus->dma->ops->add_status(s->bus->dma, BM_STATUS_INT);
   ide_set_inactive(s);
}
