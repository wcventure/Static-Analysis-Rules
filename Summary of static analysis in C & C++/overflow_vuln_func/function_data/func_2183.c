void bdrv_round_to_clusters(BlockDriverState *bs,
                            int64_t offset, unsigned int bytes,
                            int64_t *cluster_offset,
                            unsigned int *cluster_bytes)
{
    BlockDriverInfo bdi;

    if (bdrv_get_info(bs, &bdi) < 0 || bdi.cluster_size == 0) {
        *cluster_offset = offset;
        *cluster_bytes = bytes;
    } else {
        int64_t c = bdi.cluster_size;
        *cluster_offset = QEMU_ALIGN_DOWN(offset, c);
        *cluster_bytes = QEMU_ALIGN_UP(offset - *cluster_offset + bytes, c);
    }
}
