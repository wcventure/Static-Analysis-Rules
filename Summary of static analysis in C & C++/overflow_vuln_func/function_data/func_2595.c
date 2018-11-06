static BlockDriverAIOCB *rbd_start_aio(BlockDriverState *bs,
                                       int64_t sector_num,
                                       QEMUIOVector *qiov,
                                       int nb_sectors,
                                       BlockDriverCompletionFunc *cb,
                                       void *opaque,
                                       RBDAIOCmd cmd)
{
    RBDAIOCB *acb;
    RADOSCB *rcb = NULL;
    rbd_completion_t c;
    int64_t off, size;
    char *buf;
    int r;

    BDRVRBDState *s = bs->opaque;

    acb = qemu_aio_get(&rbd_aiocb_info, bs, cb, opaque);
    acb->cmd = cmd;
    acb->qiov = qiov;
    if (cmd == RBD_AIO_DISCARD || cmd == RBD_AIO_FLUSH) {
        acb->bounce = NULL;
    } else {
        acb->bounce = qemu_try_blockalign(bs, qiov->size);
        if (acb->bounce == NULL) {
            goto failed;
        }
    }
    acb->ret = 0;
    acb->error = 0;
    acb->s = s;
    acb->cancelled = 0;
    acb->bh = NULL;
    acb->status = -EINPROGRESS;

    if (cmd == RBD_AIO_WRITE) {
        qemu_iovec_to_buf(acb->qiov, 0, acb->bounce, qiov->size);
    }

    buf = acb->bounce;

    off = sector_num * BDRV_SECTOR_SIZE;
    size = nb_sectors * BDRV_SECTOR_SIZE;

    rcb = g_malloc(sizeof(RADOSCB));
    rcb->done = 0;
    rcb->acb = acb;
    rcb->buf = buf;
    rcb->s = acb->s;
    rcb->size = size;
    r = rbd_aio_create_completion(rcb, (rbd_callback_t) rbd_finish_aiocb, &c);
    if (r < 0) {
        goto failed;
    }

    switch (cmd) {
    case RBD_AIO_WRITE:
        r = rbd_aio_write(s->image, off, size, buf, c);
        break;
    case RBD_AIO_READ:
        r = rbd_aio_read(s->image, off, size, buf, c);
        break;
    case RBD_AIO_DISCARD:
        r = rbd_aio_discard_wrapper(s->image, off, size, c);
        break;
    case RBD_AIO_FLUSH:
        r = rbd_aio_flush_wrapper(s->image, c);
        break;
    default:
        r = -EINVAL;
    }

    if (r < 0) {
        goto failed_completion;
    }

    return &acb->common;

failed_completion:
    rbd_aio_release(c);
failed:
    g_free(rcb);
    qemu_vfree(acb->bounce);
    qemu_aio_release(acb);
    return NULL;
}
