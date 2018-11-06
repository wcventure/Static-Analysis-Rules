BlockDriverState *bdrv_new(const char *device_name, Error **errp)
{
    BlockDriverState *bs;
    int i;

    if (bdrv_find(device_name)) {
        error_setg(errp, "Device with id '%s' already exists",
                   device_name);
        return NULL;
    }
    if (bdrv_find_node(device_name)) {
        error_setg(errp, "Device with node-name '%s' already exists",
                   device_name);
        return NULL;
    }

    bs = g_malloc0(sizeof(BlockDriverState));
    QLIST_INIT(&bs->dirty_bitmaps);
    pstrcpy(bs->device_name, sizeof(bs->device_name), device_name);
    if (device_name[0] != '\0') {
        QTAILQ_INSERT_TAIL(&bdrv_states, bs, device_list);
    }
    for (i = 0; i < BLOCK_OP_TYPE_MAX; i++) {
        QLIST_INIT(&bs->op_blockers[i]);
    }
    bdrv_iostatus_disable(bs);
    notifier_list_init(&bs->close_notifiers);
    notifier_with_return_list_init(&bs->before_write_notifiers);
    qemu_co_queue_init(&bs->throttled_reqs[0]);
    qemu_co_queue_init(&bs->throttled_reqs[1]);
    bs->refcnt = 1;
    bs->aio_context = qemu_get_aio_context();

    return bs;
}
