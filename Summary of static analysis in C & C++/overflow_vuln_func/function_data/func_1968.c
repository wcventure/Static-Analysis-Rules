static int qcow2_write_compressed(BlockDriverState *bs, int64_t sector_num,
                                  const uint8_t *buf, int nb_sectors)
{
    BDRVQcowState *s = bs->opaque;
    z_stream strm;
    int ret, out_len;
    uint8_t *out_buf;
    uint64_t cluster_offset;

    if (nb_sectors == 0) {
        /
        cluster_offset = bdrv_getlength(bs->file);
        cluster_offset = (cluster_offset + 511) & ~511;
        bdrv_truncate(bs->file, cluster_offset);
        return 0;
    }

    if (nb_sectors != s->cluster_sectors) {
        ret = -EINVAL;

        /
        if (sector_num + nb_sectors == bs->total_sectors &&
            nb_sectors < s->cluster_sectors) {
            uint8_t *pad_buf = qemu_blockalign(bs, s->cluster_size);
            memset(pad_buf, 0, s->cluster_size);
            memcpy(pad_buf, buf, nb_sectors * BDRV_SECTOR_SIZE);
            ret = qcow2_write_compressed(bs, sector_num,
                                         pad_buf, s->cluster_sectors);
            qemu_vfree(pad_buf);
        }
        return ret;
    }

    out_buf = g_malloc(s->cluster_size + (s->cluster_size / 1000) + 128);

    /
    memset(&strm, 0, sizeof(strm));
    ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION,
                       Z_DEFLATED, -12,
                       9, Z_DEFAULT_STRATEGY);
    if (ret != 0) {
        ret = -EINVAL;
        goto fail;
    }

    strm.avail_in = s->cluster_size;
    strm.next_in = (uint8_t *)buf;
    strm.avail_out = s->cluster_size;
    strm.next_out = out_buf;

    ret = deflate(&strm, Z_FINISH);
    if (ret != Z_STREAM_END && ret != Z_OK) {
        deflateEnd(&strm);
        ret = -EINVAL;
        goto fail;
    }
    out_len = strm.next_out - out_buf;

    deflateEnd(&strm);

    if (ret != Z_STREAM_END || out_len >= s->cluster_size) {
        /
        ret = bdrv_write(bs, sector_num, buf, s->cluster_sectors);
        if (ret < 0) {
            goto fail;
        }
    } else {
        cluster_offset = qcow2_alloc_compressed_cluster_offset(bs,
            sector_num << 9, out_len);
        if (!cluster_offset) {
            ret = -EIO;
            goto fail;
        }
        cluster_offset &= s->cluster_offset_mask;

        ret = qcow2_pre_write_overlap_check(bs, QCOW2_OL_DEFAULT,
                cluster_offset, out_len);
        if (ret < 0) {
            goto fail;
        }

        BLKDBG_EVENT(bs->file, BLKDBG_WRITE_COMPRESSED);
        ret = bdrv_pwrite(bs->file, cluster_offset, out_buf, out_len);
        if (ret < 0) {
            goto fail;
        }
    }

    ret = 0;
fail:
    g_free(out_buf);
    return ret;
}
