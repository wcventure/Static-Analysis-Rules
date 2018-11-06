int qcow2_read_snapshots(BlockDriverState *bs)
{
    BDRVQcowState *s = bs->opaque;
    QCowSnapshotHeader h;
    QCowSnapshotExtraData extra;
    QCowSnapshot *sn;
    int i, id_str_size, name_size;
    int64_t offset;
    uint32_t extra_data_size;
    int ret;

    if (!s->nb_snapshots) {
        s->snapshots = NULL;
        s->snapshots_size = 0;
        return 0;
    }

    offset = s->snapshots_offset;
    s->snapshots = g_malloc0(s->nb_snapshots * sizeof(QCowSnapshot));

    for(i = 0; i < s->nb_snapshots; i++) {
        /
        offset = align_offset(offset, 8);
        ret = bdrv_pread(bs->file, offset, &h, sizeof(h));
        if (ret < 0) {
            goto fail;
        }

        offset += sizeof(h);
        sn = s->snapshots + i;
        sn->l1_table_offset = be64_to_cpu(h.l1_table_offset);
        sn->l1_size = be32_to_cpu(h.l1_size);
        sn->vm_state_size = be32_to_cpu(h.vm_state_size);
        sn->date_sec = be32_to_cpu(h.date_sec);
        sn->date_nsec = be32_to_cpu(h.date_nsec);
        sn->vm_clock_nsec = be64_to_cpu(h.vm_clock_nsec);
        extra_data_size = be32_to_cpu(h.extra_data_size);

        id_str_size = be16_to_cpu(h.id_str_size);
        name_size = be16_to_cpu(h.name_size);

        /
        ret = bdrv_pread(bs->file, offset, &extra,
                         MIN(sizeof(extra), extra_data_size));
        if (ret < 0) {
            goto fail;
        }
        offset += extra_data_size;

        if (extra_data_size >= 8) {
            sn->vm_state_size = be64_to_cpu(extra.vm_state_size_large);
        }

        if (extra_data_size >= 16) {
            sn->disk_size = be64_to_cpu(extra.disk_size);
        } else {
            sn->disk_size = bs->total_sectors * BDRV_SECTOR_SIZE;
        }

        /
        sn->id_str = g_malloc(id_str_size + 1);
        ret = bdrv_pread(bs->file, offset, sn->id_str, id_str_size);
        if (ret < 0) {
            goto fail;
        }
        offset += id_str_size;
        sn->id_str[id_str_size] = '\0';

        /
        sn->name = g_malloc(name_size + 1);
        ret = bdrv_pread(bs->file, offset, sn->name, name_size);
        if (ret < 0) {
            goto fail;
        }
        offset += name_size;
        sn->name[name_size] = '\0';

        if (offset - s->snapshots_offset > QCOW_MAX_SNAPSHOTS_SIZE) {
            ret = -EFBIG;
            goto fail;
        }
    }

    assert(offset - s->snapshots_offset <= INT_MAX);
    s->snapshots_size = offset - s->snapshots_offset;
    return 0;

fail:
    qcow2_free_snapshots(bs);
    return ret;
}
