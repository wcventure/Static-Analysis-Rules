void hmp_info_snapshots(Monitor *mon, const QDict *qdict)
{
    BlockDriverState *bs, *bs1;
    QEMUSnapshotInfo *sn_tab, *sn, s, *sn_info = &s;
    int nb_sns, i, ret, available;
    int total;
    int *available_snapshots;

    bs = find_vmstate_bs();
    if (!bs) {
        monitor_printf(mon, "No available block device supports snapshots\n");
        return;
    }

    nb_sns = bdrv_snapshot_list(bs, &sn_tab);
    if (nb_sns < 0) {
        monitor_printf(mon, "bdrv_snapshot_list: error %d\n", nb_sns);
        return;
    }

    if (nb_sns == 0) {
        monitor_printf(mon, "There is no snapshot available.\n");
        return;
    }

    available_snapshots = g_malloc0(sizeof(int) * nb_sns);
    total = 0;
    for (i = 0; i < nb_sns; i++) {
        sn = &sn_tab[i];
        available = 1;
        bs1 = NULL;

        while ((bs1 = bdrv_next(bs1))) {
            if (bdrv_can_snapshot(bs1) && bs1 != bs) {
                ret = bdrv_snapshot_find(bs1, sn_info, sn->id_str);
                if (ret < 0) {
                    available = 0;
                    break;
                }
            }
        }

        if (available) {
            available_snapshots[total] = i;
            total++;
        }
    }

    if (total > 0) {
        bdrv_snapshot_dump((fprintf_function)monitor_printf, mon, NULL);
        monitor_printf(mon, "\n");
        for (i = 0; i < total; i++) {
            sn = &sn_tab[available_snapshots[i]];
            bdrv_snapshot_dump((fprintf_function)monitor_printf, mon, sn);
            monitor_printf(mon, "\n");
        }
    } else {
        monitor_printf(mon, "There is no suitable snapshot available\n");
    }

    g_free(sn_tab);
    g_free(available_snapshots);

}
