static bool is_sector_request_lun_aligned(int64_t sector_num, int nb_sectors,
                                          IscsiLun *iscsilun)
{
    assert(nb_sectors < BDRV_REQUEST_MAX_SECTORS);
    return is_byte_request_lun_aligned(sector_num << BDRV_SECTOR_BITS,
                                       nb_sectors << BDRV_SECTOR_BITS,
                                       iscsilun);
}
