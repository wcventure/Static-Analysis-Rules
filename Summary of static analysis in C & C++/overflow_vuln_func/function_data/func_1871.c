static int vmdk_write(BlockDriverState *bs, int64_t sector_num,
                      const uint8_t *buf, int nb_sectors,
                      bool zeroed, bool zero_dry_run)
{
    BDRVVmdkState *s = bs->opaque;
    VmdkExtent *extent = NULL;
    int n, ret;
    int64_t index_in_cluster;
    uint64_t extent_begin_sector, extent_relative_sector_num;
    uint64_t cluster_offset;
    VmdkMetaData m_data;

    if (sector_num > bs->total_sectors) {
        error_report("Wrong offset: sector_num=0x%" PRIx64
                " total_sectors=0x%" PRIx64 "\n",
                sector_num, bs->total_sectors);
        return -EIO;
    }

    while (nb_sectors > 0) {
        extent = find_extent(s, sector_num, extent);
        if (!extent) {
            return -EIO;
        }
        ret = get_cluster_offset(
                                bs,
                                extent,
                                &m_data,
                                sector_num << 9, !extent->compressed,
                                &cluster_offset);
        if (extent->compressed) {
            if (ret == VMDK_OK) {
                /
                error_report("Could not write to allocated cluster"
                              " for streamOptimized");
                return -EIO;
            } else {
                /
                ret = get_cluster_offset(
                                        bs,
                                        extent,
                                        &m_data,
                                        sector_num << 9, 1,
                                        &cluster_offset);
            }
        }
        if (ret == VMDK_ERROR) {
            return -EINVAL;
        }
        extent_begin_sector = extent->end_sector - extent->sectors;
        extent_relative_sector_num = sector_num - extent_begin_sector;
        index_in_cluster = extent_relative_sector_num % extent->cluster_sectors;
        n = extent->cluster_sectors - index_in_cluster;
        if (n > nb_sectors) {
            n = nb_sectors;
        }
        if (zeroed) {
            /
            if (extent->has_zero_grain &&
                    index_in_cluster == 0 &&
                    n >= extent->cluster_sectors) {
                n = extent->cluster_sectors;
                if (!zero_dry_run) {
                    m_data.offset = VMDK_GTE_ZEROED;
                    /
                    if (vmdk_L2update(extent, &m_data) != VMDK_OK) {
                        return -EIO;
                    }
                }
            } else {
                return -ENOTSUP;
            }
        } else {
            ret = vmdk_write_extent(extent,
                            cluster_offset, index_in_cluster * 512,
                            buf, n, sector_num);
            if (ret) {
                return ret;
            }
            if (m_data.valid) {
                /
                if (vmdk_L2update(extent, &m_data) != VMDK_OK) {
                    return -EIO;
                }
            }
        }
        nb_sectors -= n;
        sector_num += n;
        buf += n * 512;

        /
        if (!s->cid_updated) {
            ret = vmdk_write_cid(bs, time(NULL));
            if (ret < 0) {
                return ret;
            }
            s->cid_updated = true;
        }
    }
    return 0;
}
