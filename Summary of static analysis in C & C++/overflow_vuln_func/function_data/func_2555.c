static int rdma_add_block(RDMAContext *rdma, const char *block_name,
                         void *host_addr,
                         ram_addr_t block_offset, uint64_t length)
{
    RDMALocalBlocks *local = &rdma->local_ram_blocks;
    RDMALocalBlock *block;
    RDMALocalBlock *old = local->block;

    local->block = g_malloc0(sizeof(RDMALocalBlock) * (local->nb_blocks + 1));

    if (local->nb_blocks) {
        int x;

        if (rdma->blockmap) {
            for (x = 0; x < local->nb_blocks; x++) {
                g_hash_table_remove(rdma->blockmap,
                                    (void *)(uintptr_t)old[x].offset);
                g_hash_table_insert(rdma->blockmap,
                                    (void *)(uintptr_t)old[x].offset,
                                    &local->block[x]);
            }
        }
        memcpy(local->block, old, sizeof(RDMALocalBlock) * local->nb_blocks);
        g_free(old);
    }

    block = &local->block[local->nb_blocks];

    block->block_name = g_strdup(block_name);
    block->local_host_addr = host_addr;
    block->offset = block_offset;
    block->length = length;
    block->index = local->nb_blocks;
    block->src_index = ~0U; /
    block->nb_chunks = ram_chunk_index(host_addr, host_addr + length) + 1UL;
    block->transit_bitmap = bitmap_new(block->nb_chunks);
    bitmap_clear(block->transit_bitmap, 0, block->nb_chunks);
    block->unregister_bitmap = bitmap_new(block->nb_chunks);
    bitmap_clear(block->unregister_bitmap, 0, block->nb_chunks);
    block->remote_keys = g_malloc0(block->nb_chunks * sizeof(uint32_t));

    block->is_ram_block = local->init ? false : true;

    if (rdma->blockmap) {
        g_hash_table_insert(rdma->blockmap, (void *) block_offset, block);
    }

    trace_rdma_add_block(block_name, local->nb_blocks,
                         (uintptr_t) block->local_host_addr,
                         block->offset, block->length,
                         (uintptr_t) (block->local_host_addr + block->length),
                         BITS_TO_LONGS(block->nb_chunks) *
                             sizeof(unsigned long) * 8,
                         block->nb_chunks);

    local->nb_blocks++;

    return 0;
}
