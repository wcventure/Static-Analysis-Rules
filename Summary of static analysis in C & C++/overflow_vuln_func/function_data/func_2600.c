static int vhdx_create_new_headers(BlockDriverState *bs, uint64_t image_size,
                                   uint32_t log_size)
{
    int ret = 0;
    VHDXHeader *hdr = NULL;

    hdr = g_malloc0(sizeof(VHDXHeader));

    hdr->signature       = VHDX_HEADER_SIGNATURE;
    hdr->sequence_number = g_random_int();
    hdr->log_version     = 0;
    hdr->version         = 1;
    hdr->log_length      = log_size;
    hdr->log_offset      = VHDX_HEADER_SECTION_END;
    vhdx_guid_generate(&hdr->file_write_guid);
    vhdx_guid_generate(&hdr->data_write_guid);

    ret = vhdx_write_header(bs, hdr, VHDX_HEADER1_OFFSET, false);
    if (ret < 0) {
        goto exit;
    }
    hdr->sequence_number++;
    ret = vhdx_write_header(bs, hdr, VHDX_HEADER2_OFFSET, false);
    if (ret < 0) {
        goto exit;
    }

exit:
    g_free(hdr);
    return ret;
}
