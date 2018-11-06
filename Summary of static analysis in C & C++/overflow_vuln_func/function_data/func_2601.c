static int vhdx_create_new_region_table(BlockDriverState *bs,
                                        uint64_t image_size,
                                        uint32_t block_size,
                                        uint32_t sector_size,
                                        uint32_t log_size,
                                        bool use_zero_blocks,
                                        VHDXImageType type,
                                        uint64_t *metadata_offset)
{
    int ret = 0;
    uint32_t offset = 0;
    void *buffer = NULL;
    uint64_t bat_file_offset;
    uint32_t bat_length;
    BDRVVHDXState *s = NULL;
    VHDXRegionTableHeader *region_table;
    VHDXRegionTableEntry *rt_bat;
    VHDXRegionTableEntry *rt_metadata;

    assert(metadata_offset != NULL);

    /
    s = g_malloc0(sizeof(BDRVVHDXState));

    s->chunk_ratio = (VHDX_MAX_SECTORS_PER_BLOCK) *
                     (uint64_t) sector_size / (uint64_t) block_size;

    s->sectors_per_block = block_size / sector_size;
    s->virtual_disk_size = image_size;
    s->block_size = block_size;
    s->logical_sector_size = sector_size;

    vhdx_set_shift_bits(s);

    vhdx_calc_bat_entries(s);

    /

    /
    buffer = g_malloc0(VHDX_HEADER_BLOCK_SIZE);
    region_table = buffer;
    offset += sizeof(VHDXRegionTableHeader);
    rt_bat = buffer + offset;
    offset += sizeof(VHDXRegionTableEntry);
    rt_metadata  = buffer + offset;

    region_table->signature = VHDX_REGION_SIGNATURE;
    region_table->entry_count = 2;   /

    rt_bat->guid        = bat_guid;
    rt_bat->length      = ROUND_UP(s->bat_entries * sizeof(VHDXBatEntry), MiB);
    rt_bat->file_offset = ROUND_UP(VHDX_HEADER_SECTION_END + log_size, MiB);
    s->bat_offset = rt_bat->file_offset;

    rt_metadata->guid        = metadata_guid;
    rt_metadata->file_offset = ROUND_UP(rt_bat->file_offset + rt_bat->length,
                                        MiB);
    rt_metadata->length      = 1 * MiB; /
    *metadata_offset = rt_metadata->file_offset;

    bat_file_offset = rt_bat->file_offset;
    bat_length = rt_bat->length;

    vhdx_region_header_le_export(region_table);
    vhdx_region_entry_le_export(rt_bat);
    vhdx_region_entry_le_export(rt_metadata);

    vhdx_update_checksum(buffer, VHDX_HEADER_BLOCK_SIZE,
                         offsetof(VHDXRegionTableHeader, checksum));


    /
    ret = vhdx_create_bat(bs, s, image_size, type, use_zero_blocks,
                          bat_file_offset, bat_length);
    if (ret < 0) {
        goto exit;
    }

    /
    ret = bdrv_pwrite(bs, VHDX_REGION_TABLE_OFFSET, buffer,
                      VHDX_HEADER_BLOCK_SIZE);
    if (ret < 0) {
        goto exit;
    }

    ret = bdrv_pwrite(bs, VHDX_REGION_TABLE2_OFFSET, buffer,
                      VHDX_HEADER_BLOCK_SIZE);
    if (ret < 0) {
        goto exit;
    }


exit:
    g_free(s);
    g_free(buffer);
    return ret;
}
