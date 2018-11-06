static int save_xbzrle_page(QEMUFile *f, uint8_t **current_data,
                            ram_addr_t current_addr, RAMBlock *block,
                            ram_addr_t offset, int cont, bool last_stage)
{
    int encoded_len = 0, bytes_sent = -1;
    uint8_t *prev_cached_page;

    if (!cache_is_cached(XBZRLE.cache, current_addr)) {
        acct_info.xbzrle_cache_miss++;
        if (!last_stage) {
            if (cache_insert(XBZRLE.cache, current_addr, *current_data) == -1) {
                return -1;
            } else {
                /
                *current_data = get_cached_data(XBZRLE.cache, current_addr);
            }
        }
        return -1;
    }

    prev_cached_page = get_cached_data(XBZRLE.cache, current_addr);

    /
    memcpy(XBZRLE.current_buf, *current_data, TARGET_PAGE_SIZE);

    /
    encoded_len = xbzrle_encode_buffer(prev_cached_page, XBZRLE.current_buf,
                                       TARGET_PAGE_SIZE, XBZRLE.encoded_buf,
                                       TARGET_PAGE_SIZE);
    if (encoded_len == 0) {
        DPRINTF("Skipping unmodified page\n");
        return 0;
    } else if (encoded_len == -1) {
        DPRINTF("Overflow\n");
        acct_info.xbzrle_overflows++;
        /
        if (!last_stage) {
            memcpy(prev_cached_page, *current_data, TARGET_PAGE_SIZE);
            *current_data = prev_cached_page;
        }
        return -1;
    }

    /
    if (!last_stage) {
        memcpy(prev_cached_page, XBZRLE.current_buf, TARGET_PAGE_SIZE);
    }

    /
    bytes_sent = save_block_hdr(f, block, offset, cont, RAM_SAVE_FLAG_XBZRLE);
    qemu_put_byte(f, ENCODING_FLAG_XBZRLE);
    qemu_put_be16(f, encoded_len);
    qemu_put_buffer(f, XBZRLE.encoded_buf, encoded_len);
    bytes_sent += encoded_len + 1 + 2;
    acct_info.xbzrle_pages++;
    acct_info.xbzrle_bytes += bytes_sent;

    return bytes_sent;
}
