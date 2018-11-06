int qcow2_snapshot_list(BlockDriverState *bs, QEMUSnapshotInfo **psn_tab)
{
    BDRVQcowState *s = bs->opaque;
    QEMUSnapshotInfo *sn_tab, *sn_info;
    QCowSnapshot *sn;
    int i;

    if (!s->nb_snapshots) {
        *psn_tab = NULL;
        return s->nb_snapshots;
    }

    sn_tab = g_malloc0(s->nb_snapshots * sizeof(QEMUSnapshotInfo));
    for(i = 0; i < s->nb_snapshots; i++) {
        sn_info = sn_tab + i;
        sn = s->snapshots + i;
        pstrcpy(sn_info->id_str, sizeof(sn_info->id_str),
                sn->id_str);
        pstrcpy(sn_info->name, sizeof(sn_info->name),
                sn->name);
        sn_info->vm_state_size = sn->vm_state_size;
        sn_info->date_sec = sn->date_sec;
        sn_info->date_nsec = sn->date_nsec;
        sn_info->vm_clock_nsec = sn->vm_clock_nsec;
    }
    *psn_tab = sn_tab;
    return s->nb_snapshots;
}
