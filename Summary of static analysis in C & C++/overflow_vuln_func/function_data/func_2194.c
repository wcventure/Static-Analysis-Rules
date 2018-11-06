void virtio_blk_submit_multireq(BlockBackend *blk, MultiReqBuffer *mrb)
{
    int i = 0, start = 0, num_reqs = 0, niov = 0, nb_sectors = 0;
    int max_xfer_len = 0;
    int64_t sector_num = 0;

    if (mrb->num_reqs == 1) {
        submit_requests(blk, mrb, 0, 1, -1);
        mrb->num_reqs = 0;
        return;
    }

    max_xfer_len = blk_get_max_transfer_length(mrb->reqs[0]->dev->blk);
    max_xfer_len = MIN_NON_ZERO(max_xfer_len, BDRV_REQUEST_MAX_SECTORS);

    qsort(mrb->reqs, mrb->num_reqs, sizeof(*mrb->reqs),
          &multireq_compare);

    for (i = 0; i < mrb->num_reqs; i++) {
        VirtIOBlockReq *req = mrb->reqs[i];
        if (num_reqs > 0) {
            bool merge = true;

            /
            if (niov + req->qiov.niov > IOV_MAX) {
                merge = false;
            }

            /
            if (req->qiov.size / BDRV_SECTOR_SIZE + nb_sectors > max_xfer_len) {
                merge = false;
            }

            /
            if (sector_num + nb_sectors != req->sector_num) {
                merge = false;
            }

            if (!merge) {
                submit_requests(blk, mrb, start, num_reqs, niov);
                num_reqs = 0;
            }
        }

        if (num_reqs == 0) {
            sector_num = req->sector_num;
            nb_sectors = niov = 0;
            start = i;
        }

        nb_sectors += req->qiov.size / BDRV_SECTOR_SIZE;
        niov += req->qiov.niov;
        num_reqs++;
    }

    submit_requests(blk, mrb, start, num_reqs, niov);
    mrb->num_reqs = 0;
}
