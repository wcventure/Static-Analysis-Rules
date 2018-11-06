static int64_t archipelago_volume_info(BDRVArchipelagoState *s)
{
    uint64_t size;
    int ret, targetlen;
    struct xseg_request *req;
    struct xseg_reply_info *xinfo;
    AIORequestData *reqdata = g_malloc(sizeof(AIORequestData));

    const char *volname = s->volname;
    targetlen = strlen(volname);
    req = xseg_get_request(s->xseg, s->srcport, s->mportno, X_ALLOC);
    if (!req) {
        archipelagolog("Cannot get XSEG request\n");
        goto err_exit2;
    }
    ret = xseg_prep_request(s->xseg, req, targetlen,
                            sizeof(struct xseg_reply_info));
    if (ret < 0) {
        archipelagolog("Cannot prepare XSEG request\n");
        goto err_exit;
    }
    char *target = xseg_get_target(s->xseg, req);
    if (!target) {
        archipelagolog("Cannot get XSEG target\n");
        goto err_exit;
    }
    memcpy(target, volname, targetlen);
    req->size = req->datalen;
    req->offset = 0;
    req->op = X_INFO;

    reqdata->op = ARCHIP_OP_VOLINFO;
    reqdata->volname = volname;
    xseg_set_req_data(s->xseg, req, reqdata);

    xport p = xseg_submit(s->xseg, req, s->srcport, X_ALLOC);
    if (p == NoPort) {
        archipelagolog("Cannot submit XSEG request\n");
        goto err_exit;
    }
    xseg_signal(s->xseg, p);
    qemu_mutex_lock(&s->archip_mutex);
    while (!s->is_signaled) {
        qemu_cond_wait(&s->archip_cond, &s->archip_mutex);
    }
    s->is_signaled = false;
    qemu_mutex_unlock(&s->archip_mutex);

    xinfo = (struct xseg_reply_info *) xseg_get_data(s->xseg, req);
    size = xinfo->size;
    xseg_put_request(s->xseg, req, s->srcport);
    g_free(reqdata);
    s->size = size;
    return size;

err_exit:
    xseg_put_request(s->xseg, req, s->srcport);
err_exit2:
    g_free(reqdata);
    return -EIO;
}
