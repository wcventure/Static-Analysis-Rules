static int ioreq_runio_qemu_aio(struct ioreq *ioreq)
{
    struct XenBlkDev *blkdev = ioreq->blkdev;

    if (ioreq->blkdev->feature_grant_copy) {
        ioreq_init_copy_buffers(ioreq);
        if (ioreq->req.nr_segments && (ioreq->req.operation == BLKIF_OP_WRITE ||
            ioreq->req.operation == BLKIF_OP_FLUSH_DISKCACHE) &&
            ioreq_grant_copy(ioreq)) {
                ioreq_free_copy_buffers(ioreq);
                goto err;
        }
    } else {
        if (ioreq->req.nr_segments && ioreq_map(ioreq)) {
            goto err;
        }
    }

    ioreq->aio_inflight++;
    if (ioreq->presync) {
        blk_aio_flush(ioreq->blkdev->blk, qemu_aio_complete, ioreq);
        return 0;
    }

    switch (ioreq->req.operation) {
    case BLKIF_OP_READ:
        block_acct_start(blk_get_stats(blkdev->blk), &ioreq->acct,
                         ioreq->v.size, BLOCK_ACCT_READ);
        ioreq->aio_inflight++;
        blk_aio_preadv(blkdev->blk, ioreq->start, &ioreq->v, 0,
                       qemu_aio_complete, ioreq);
        break;
    case BLKIF_OP_WRITE:
    case BLKIF_OP_FLUSH_DISKCACHE:
        if (!ioreq->req.nr_segments) {
            break;
        }

        block_acct_start(blk_get_stats(blkdev->blk), &ioreq->acct,
                         ioreq->v.size,
                         ioreq->req.operation == BLKIF_OP_WRITE ?
                         BLOCK_ACCT_WRITE : BLOCK_ACCT_FLUSH);
        ioreq->aio_inflight++;
        blk_aio_pwritev(blkdev->blk, ioreq->start, &ioreq->v, 0,
                        qemu_aio_complete, ioreq);
        break;
    case BLKIF_OP_DISCARD:
    {
        struct blkif_request_discard *discard_req = (void *)&ioreq->req;
        ioreq->aio_inflight++;
        blk_aio_pdiscard(blkdev->blk,
                         discard_req->sector_number << BDRV_SECTOR_BITS,
                         discard_req->nr_sectors << BDRV_SECTOR_BITS,
                         qemu_aio_complete, ioreq);
        break;
    }
    default:
        /
        if (!ioreq->blkdev->feature_grant_copy) {
            ioreq_unmap(ioreq);
        }
        goto err;
    }

    qemu_aio_complete(ioreq, 0);

    return 0;

err:
    ioreq_finish(ioreq);
    ioreq->status = BLKIF_RSP_ERROR;
    return -1;
}
