static int vmdk_open_vmfs_sparse(BlockDriverState *bs,
                                 BlockDriverState *file,
                                 int flags, Error **errp)
{
    int ret;
    uint32_t magic;
    VMDK3Header header;
    VmdkExtent *extent;

    ret = bdrv_pread(file, sizeof(magic), &header, sizeof(header));
    if (ret < 0) {
        error_setg_errno(errp, -ret,
                         "Could not read header from file '%s'",
                         file->filename);
        return ret;
    }
    ret = vmdk_add_extent(bs, file, false,
                          le32_to_cpu(header.disk_sectors),
                          le32_to_cpu(header.l1dir_offset) << 9,
                          0,
                          le32_to_cpu(header.l1dir_size),
                          4096,
                          le32_to_cpu(header.granularity),
                          &extent,
                          errp);
    if (ret < 0) {
        return ret;
    }
    ret = vmdk_init_tables(bs, extent, errp);
    if (ret) {
        /
        vmdk_free_last_extent(bs);
    }
    return ret;
}
