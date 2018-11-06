static int qemu_rbd_snap_list(BlockDriverState *bs,
                              QEMUSnapshotInfo **psn_tab)
{
    BDRVRBDState *s = bs->opaque;
    QEMUSnapshotInfo *sn_info, *sn_tab = NULL;
    int i, snap_count;
    rbd_snap_info_t *snaps;
    int max_snaps = RBD_MAX_SNAPS;

    do {
        snaps = g_malloc(sizeof(*snaps) * max_snaps);
        snap_count = rbd_snap_list(s->image, snaps, &max_snaps);
        if (snap_count <= 0) {
            g_free(snaps);
        }
    } while (snap_count == -ERANGE);

    if (snap_count <= 0) {
        goto done;
    }

    sn_tab = g_new0(QEMUSnapshotInfo, snap_count);

    for (i = 0; i < snap_count; i++) {
        const char *snap_name = snaps[i].name;

        sn_info = sn_tab + i;
        pstrcpy(sn_info->id_str, sizeof(sn_info->id_str), snap_name);
        pstrcpy(sn_info->name, sizeof(sn_info->name), snap_name);

        sn_info->vm_state_size = snaps[i].size;
        sn_info->date_sec = 0;
        sn_info->date_nsec = 0;
        sn_info->vm_clock_nsec = 0;
    }
    rbd_snap_list_end(snaps);
    g_free(snaps);

 done:
    *psn_tab = sn_tab;
    return snap_count;
}
