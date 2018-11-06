static int ram_save_block(QEMUFile *f, bool last_stage)
{
    RAMBlock *block = last_seen_block;
    ram_addr_t offset = last_offset;
    bool complete_round = false;
    int bytes_sent = 0;
    MemoryRegion *mr;
    ram_addr_t current_addr;

    if (!block)
        block = QTAILQ_FIRST(&ram_list.blocks);

    while (true) {
        mr = block->mr;
        offset = migration_bitmap_find_and_reset_dirty(mr, offset);
        if (complete_round && block == last_seen_block &&
            offset >= last_offset) {
            break;
        }
        if (offset >= block->length) {
            offset = 0;
            block = QTAILQ_NEXT(block, next);
            if (!block) {
                block = QTAILQ_FIRST(&ram_list.blocks);
                complete_round = true;
                ram_bulk_stage = false;
            }
        } else {
            uint8_t *p;
            int cont = (block == last_sent_block) ?
                RAM_SAVE_FLAG_CONTINUE : 0;

            p = memory_region_get_ram_ptr(mr) + offset;

            /
            bytes_sent = -1;
            if (is_zero_page(p)) {
                acct_info.dup_pages++;
                if (!ram_bulk_stage) {
                    bytes_sent = save_block_hdr(f, block, offset, cont,
                                                RAM_SAVE_FLAG_COMPRESS);
                    qemu_put_byte(f, 0);
                    bytes_sent++;
                } else {
                    acct_info.skipped_pages++;
                    bytes_sent = 0;
                }
            } else if (migrate_use_xbzrle()) {
                current_addr = block->offset + offset;
                bytes_sent = save_xbzrle_page(f, p, current_addr, block,
                                              offset, cont, last_stage);
                if (!last_stage) {
                    p = get_cached_data(XBZRLE.cache, current_addr);
                }
            }

            /
            if (bytes_sent == -1) {
                bytes_sent = save_block_hdr(f, block, offset, cont, RAM_SAVE_FLAG_PAGE);
                qemu_put_buffer(f, p, TARGET_PAGE_SIZE);
                bytes_sent += TARGET_PAGE_SIZE;
                acct_info.norm_pages++;
            }

            /
            if (bytes_sent > 0) {
                last_sent_block = block;
                break;
            }
        }
    }
    last_seen_block = block;
    last_offset = offset;

    return bytes_sent;
}
