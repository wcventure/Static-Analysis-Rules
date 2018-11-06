static coroutine_fn int qcow2_co_readv(BlockDriverState *bs, int64_t sector_num,
                          int remaining_sectors, QEMUIOVector *qiov)
{
    BDRVQcowState *s = bs->opaque;
    int index_in_cluster, n1;
    int ret;
    int cur_nr_sectors; /
    uint64_t cluster_offset = 0;
    uint64_t bytes_done = 0;
    QEMUIOVector hd_qiov;
    uint8_t *cluster_data = NULL;

    qemu_iovec_init(&hd_qiov, qiov->niov);

    qemu_co_mutex_lock(&s->lock);

    while (remaining_sectors != 0) {

        /
        cur_nr_sectors = remaining_sectors;
        if (s->crypt_method) {
            cur_nr_sectors = MIN(cur_nr_sectors,
                QCOW_MAX_CRYPT_CLUSTERS * s->cluster_sectors);
        }

        ret = qcow2_get_cluster_offset(bs, sector_num << 9,
            &cur_nr_sectors, &cluster_offset);
        if (ret < 0) {
            goto fail;
        }

        index_in_cluster = sector_num & (s->cluster_sectors - 1);

        qemu_iovec_reset(&hd_qiov);
        qemu_iovec_concat(&hd_qiov, qiov, bytes_done,
            cur_nr_sectors * 512);

        switch (ret) {
        case QCOW2_CLUSTER_UNALLOCATED:

            if (bs->backing_hd) {
                /
                n1 = qcow2_backing_read1(bs->backing_hd, &hd_qiov,
                    sector_num, cur_nr_sectors);
                if (n1 > 0) {
                    BLKDBG_EVENT(bs->file, BLKDBG_READ_BACKING_AIO);
                    qemu_co_mutex_unlock(&s->lock);
                    ret = bdrv_co_readv(bs->backing_hd, sector_num,
                                        n1, &hd_qiov);
                    qemu_co_mutex_lock(&s->lock);
                    if (ret < 0) {
                        goto fail;
                    }
                }
            } else {
                /
                qemu_iovec_memset(&hd_qiov, 0, 0, 512 * cur_nr_sectors);
            }
            break;

        case QCOW2_CLUSTER_ZERO:
            qemu_iovec_memset(&hd_qiov, 0, 0, 512 * cur_nr_sectors);
            break;

        case QCOW2_CLUSTER_COMPRESSED:
            /
            ret = qcow2_decompress_cluster(bs, cluster_offset);
            if (ret < 0) {
                goto fail;
            }

            qemu_iovec_from_buf(&hd_qiov, 0,
                s->cluster_cache + index_in_cluster * 512,
                512 * cur_nr_sectors);
            break;

        case QCOW2_CLUSTER_NORMAL:
            if ((cluster_offset & 511) != 0) {
                ret = -EIO;
                goto fail;
            }

            if (s->crypt_method) {
                /
                if (!cluster_data) {
                    cluster_data =
                        qemu_blockalign(bs, QCOW_MAX_CRYPT_CLUSTERS * s->cluster_size);
                }

                assert(cur_nr_sectors <=
                    QCOW_MAX_CRYPT_CLUSTERS * s->cluster_sectors);
                qemu_iovec_reset(&hd_qiov);
                qemu_iovec_add(&hd_qiov, cluster_data,
                    512 * cur_nr_sectors);
            }

            BLKDBG_EVENT(bs->file, BLKDBG_READ_AIO);
            qemu_co_mutex_unlock(&s->lock);
            ret = bdrv_co_readv(bs->file,
                                (cluster_offset >> 9) + index_in_cluster,
                                cur_nr_sectors, &hd_qiov);
            qemu_co_mutex_lock(&s->lock);
            if (ret < 0) {
                goto fail;
            }
            if (s->crypt_method) {
                qcow2_encrypt_sectors(s, sector_num,  cluster_data,
                    cluster_data, cur_nr_sectors, 0, &s->aes_decrypt_key);
                qemu_iovec_from_buf(qiov, bytes_done,
                    cluster_data, 512 * cur_nr_sectors);
            }
            break;

        default:
            g_assert_not_reached();
            ret = -EIO;
            goto fail;
        }

        remaining_sectors -= cur_nr_sectors;
        sector_num += cur_nr_sectors;
        bytes_done += cur_nr_sectors * 512;
    }
    ret = 0;

fail:
    qemu_co_mutex_unlock(&s->lock);

    qemu_iovec_destroy(&hd_qiov);
    qemu_vfree(cluster_data);

    return ret;
}
