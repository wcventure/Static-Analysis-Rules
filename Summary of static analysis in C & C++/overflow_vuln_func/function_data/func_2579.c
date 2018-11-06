static int archipelago_submit_request(BDRVArchipelagoState *s,
                                        uint64_t bufidx,
                                        size_t count,
                                        off_t offset,
                                        ArchipelagoAIOCB *aio_cb,
                                        ArchipelagoSegmentedRequest *segreq,
                                        int op)
{
    int ret, targetlen;
    char *target;
    void *data = NULL;
    struct xseg_request *req;
    AIORequestData *reqdata = g_malloc(sizeof(AIORequestData));

    targetlen = strlen(s->volname);
    req = xseg_get_request(s->xseg, s->srcport, s->vportno, X_ALLOC);
    if (!req) {
        archipelagolog("Cannot get XSEG request\n");
        goto err_exit2;
    }
    ret = xseg_prep_request(s->xseg, req, targetlen, count);
    if (ret < 0) {
        archipelagolog("Cannot prepare XSEG request\n");
        goto err_exit;
    }
    target = xseg_get_target(s->xseg, req);
    if (!target) {
        archipelagolog("Cannot get XSEG target\n");
        goto err_exit;
    }
    memcpy(target, s->volname, targetlen);
    req->size = count;
    req->offset = offset;

    switch (op) {
    case ARCHIP_OP_READ:
        req->op = X_READ;
        break;
    case ARCHIP_OP_WRITE:
        req->op = X_WRITE;
        break;
    case ARCHIP_OP_FLUSH:
        req->op = X_FLUSH;
        break;
    }
    reqdata->volname = s->volname;
    reqdata->offset = offset;
    reqdata->size = count;
    reqdata->bufidx = bufidx;
    reqdata->aio_cb = aio_cb;
    reqdata->segreq = segreq;
    reqdata->op = op;

    xseg_set_req_data(s->xseg, req, reqdata);
    if (op == ARCHIP_OP_WRITE) {
        data = xseg_get_data(s->xseg, req);
        if (!data) {
            archipelagolog("Cannot get XSEG data\n");
            goto err_exit;
        }
        qemu_iovec_to_buf(aio_cb->qiov, bufidx, data, count);
    }

    xport p = xseg_submit(s->xseg, req, s->srcport, X_ALLOC);
    if (p == NoPort) {
        archipelagolog("Could not submit XSEG request\n");
        goto err_exit;
    }
    xseg_signal(s->xseg, p);
    return 0;

err_exit:
    g_free(reqdata);
    xseg_put_request(s->xseg, req, s->srcport);
    return -EIO;
err_exit2:
    g_free(reqdata);
    return -EIO;
}
