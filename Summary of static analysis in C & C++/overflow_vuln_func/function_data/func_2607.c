SnapshotInfo *qmp_blockdev_snapshot_delete_internal_sync(const char *device,
                                                         bool has_id,
                                                         const char *id,
                                                         bool has_name,
                                                         const char *name,
                                                         Error **errp)
{
    BlockDriverState *bs = bdrv_find(device);
    QEMUSnapshotInfo sn;
    Error *local_err = NULL;
    SnapshotInfo *info = NULL;
    int ret;

    if (!bs) {
        error_set(errp, QERR_DEVICE_NOT_FOUND, device);
        return NULL;
    }

    if (!has_id) {
        id = NULL;
    }

    if (!has_name) {
        name = NULL;
    }

    if (!id && !name) {
        error_setg(errp, "Name or id must be provided");
        return NULL;
    }

    ret = bdrv_snapshot_find_by_id_and_name(bs, id, name, &sn, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return NULL;
    }
    if (!ret) {
        error_setg(errp,
                   "Snapshot with id '%s' and name '%s' does not exist on "
                   "device '%s'",
                   STR_OR_NULL(id), STR_OR_NULL(name), device);
        return NULL;
    }

    bdrv_snapshot_delete(bs, id, name, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return NULL;
    }

    info = g_malloc0(sizeof(SnapshotInfo));
    info->id = g_strdup(sn.id_str);
    info->name = g_strdup(sn.name);
    info->date_nsec = sn.date_nsec;
    info->date_sec = sn.date_sec;
    info->vm_state_size = sn.vm_state_size;
    info->vm_clock_nsec = sn.vm_clock_nsec % 1000000000;
    info->vm_clock_sec = sn.vm_clock_nsec / 1000000000;

    return info;
}
