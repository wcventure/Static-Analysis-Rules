void qmp_nbd_server_add(const char *device, bool has_writable, bool writable,
                        Error **errp)
{
    BlockDriverState *bs;
    NBDExport *exp;
    NBDCloseNotifier *n;

    if (server_fd == -1) {
        error_setg(errp, "NBD server not running");
        return;
    }

    if (nbd_export_find(device)) {
        error_setg(errp, "NBD server already exporting device '%s'", device);
        return;
    }

    bs = bdrv_find(device);
    if (!bs) {
        error_set(errp, QERR_DEVICE_NOT_FOUND, device);
        return;
    }
    if (!bdrv_is_inserted(bs)) {
        error_set(errp, QERR_DEVICE_HAS_NO_MEDIUM, device);
        return;
    }

    if (!has_writable) {
        writable = false;
    }
    if (bdrv_is_read_only(bs)) {
        writable = false;
    }

    exp = nbd_export_new(bs, 0, -1, writable ? 0 : NBD_FLAG_READ_ONLY, NULL);

    nbd_export_set_name(exp, device);

    n = g_malloc0(sizeof(NBDCloseNotifier));
    n->n.notify = nbd_close_notifier;
    n->exp = exp;
    bdrv_add_close_notifier(bs, &n->n);
    QTAILQ_INSERT_TAIL(&close_notifiers, n, next);
}
