static int dmg_read_resource_fork(BlockDriverState *bs, DmgHeaderState *ds,
                                  uint64_t info_begin, uint64_t info_length)
{
    BDRVDMGState *s = bs->opaque;
    int ret;
    uint32_t count, rsrc_data_offset;
    uint8_t *buffer = NULL;
    uint64_t info_end;
    uint64_t offset;

    /
    ret = read_uint32(bs, info_begin, &rsrc_data_offset);
    if (ret < 0) {
        goto fail;
    } else if (rsrc_data_offset > info_length) {
        ret = -EINVAL;
        goto fail;
    }

    /
    ret = read_uint32(bs, info_begin + 8, &count);
    if (ret < 0) {
        goto fail;
    } else if (count == 0 || rsrc_data_offset + count > info_length) {
        ret = -EINVAL;
        goto fail;
    }

    /
    offset = info_begin + rsrc_data_offset;

    /
    info_end = offset + count;

    /
    while (offset < info_end) {
        /
        ret = read_uint32(bs, offset, &count);
        if (ret < 0) {
            goto fail;
        } else if (count == 0) {
            ret = -EINVAL;
            goto fail;
        }
        offset += 4;

        buffer = g_realloc(buffer, count);
        ret = bdrv_pread(bs->file, offset, buffer, count);
        if (ret < 0) {
            goto fail;
        }

        ret = dmg_read_mish_block(s, ds, buffer, count);
        if (ret < 0) {
            goto fail;
        }
        /
        offset += count;
    }
    ret = 0;

fail:
    g_free(buffer);
    return ret;
}
