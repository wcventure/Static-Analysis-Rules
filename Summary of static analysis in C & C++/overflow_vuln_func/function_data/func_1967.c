static coroutine_fn int qcow2_co_writev(BlockDriverState *bs,
                           int64_t sector_num,
                           int remaining_sectors,
                           QEMUIOVector *qiov)
{
    BDRVQcowState *s = bs->opaque;
    int index_in_cluster;
    int n_end;
    int ret;
    int cur_nr_sectors; /
    uint64_t cluster_offset;
    QEMUIOVector hd_qiov;
    uint64_t bytes_done = 0;
    uint8_t *cluster_data = NULL;
    QCowL2Meta *l2meta = NULL;

    trace_qcow2_writev_start_req(qemu_coroutine_self(), sector_num,
                                 remaining_sectors);

    qemu_iovec_init(&hd_qiov, qiov->niov);

    s->cluster_cache_offset = -1; /

    qemu_co_mutex_lock(&s->lock);

    while (remaining_sectors != 0) {

        l2meta = NULL;

        trace_qcow2_writev_start_part(qemu_coroutine_self());
        index_in_cluster = sector_num & (s->cluster_sectors - 1);
        n_end = index_in_cluster + remaining_sectors;
        if (s->crypt_method &&
            n_end > QCOW_MAX_CRYPT_CLUSTERS * s->cluster_sectors) {
            n_end = QCOW_MAX_CRYPT_CLUSTERS * s->cluster_sectors;
        }

        ret = qcow2_alloc_cluster_offset(bs, sector_num << 9,
            index_in_cluster, n_end, &cur_nr_sectors, &cluster_offset, &l2meta);
        if (ret < 0) {
            goto fail;
        }

        assert((cluster_offset & 511) == 0);

        qemu_iovec_reset(&hd_qiov);
        qemu_iovec_concat(&hd_qiov, qiov, bytes_done,
            cur_nr_sectors * 512);

        if (s->crypt_method) {
            if (!cluster_data) {
                cluster_data = qemu_blockalign(bs, QCOW_MAX_CRYPT_CLUSTERS *
                                                 s->cluster_size);
            }

            assert(hd_qiov.size <=
                   QCOW_MAX_CRYPT_CLUSTERS * s->cluster_size);
            qemu_iovec_to_buf(&hd_qiov, 0, cluster_data, hd_qiov.size);

            qcow2_encrypt_sectors(s, sector_num, cluster_data,
                cluster_data, cur_nr_sectors, 1, &s->aes_encrypt_key);

            qemu_iovec_reset(&hd_qiov);
            qemu_iovec_add(&hd_qiov, cluster_data,
                cur_nr_sectors * 512);
        }

        ret = qcow2_pre_write_overlap_check(bs, QCOW2_OL_DEFAULT,
                cluster_offset + index_in_cluster * BDRV_SECTOR_SIZE,
                cur_nr_sectors * BDRV_SECTOR_SIZE);
        if (ret < 0) {
            goto fail;
        }

        qemu_co_mutex_unlock(&s->lock);
        BLKDBG_EVENT(bs->file, BLKDBG_WRITE_AIO);
        trace_qcow2_writev_data(qemu_coroutine_self(),
                                (cluster_offset >> 9) + index_in_cluster);
        ret = bdrv_co_writev(bs->file,
                             (cluster_offset >> 9) + index_in_cluster,
                             cur_nr_sectors, &hd_qiov);
        qemu_co_mutex_lock(&s->lock);
        if (ret < 0) {
            goto fail;
        }

        while (l2meta != NULL) {
            QCowL2Meta *next;

            ret = qcow2_alloc_cluster_link_l2(bs, l2meta);
            if (ret < 0) {
                goto fail;
            }

            /
            if (l2meta->nb_clusters != 0) {
                QLIST_REMOVE(l2meta, next_in_flight);
            }

            qemu_co_queue_restart_all(&l2meta->dependent_requests);

            next = l2meta->next;
            g_free(l2meta);
            l2meta = next;
        }

        remaining_sectors -= cur_nr_sectors;
        sector_num += cur_nr_sectors;
        bytes_done += cur_nr_sectors * 512;
        trace_qcow2_writev_done_part(qemu_coroutine_self(), cur_nr_sectors);
    }
    ret = 0;

fail:
    qemu_co_mutex_unlock(&s->lock);

    while (l2meta != NULL) {
        QCowL2Meta *next;

        if (l2meta->nb_clusters != 0) {
            QLIST_REMOVE(l2meta, next_in_flight);
        }
        qemu_co_queue_restart_all(&l2meta->dependent_requests);

        next = l2meta->next;
        g_free(l2meta);
        l2meta = next;
    }

    qemu_iovec_destroy(&hd_qiov);
    qemu_vfree(cluster_data);
    trace_qcow2_writev_done_req(qemu_coroutine_self(), ret);

    return ret;
}
