static int rdma_delete_block(RDMAContext *rdma, RDMALocalBlock *block)
{
    RDMALocalBlocks *local = &rdma->local_ram_blocks;
    RDMALocalBlock *old = local->block;
    int x;

    if (rdma->blockmap) {
        g_hash_table_remove(rdma->blockmap, (void *)(uintptr_t)block->offset);
    }
    if (block->pmr) {
        int j;

        for (j = 0; j < block->nb_chunks; j++) {
            if (!block->pmr[j]) {
                continue;
            }
            ibv_dereg_mr(block->pmr[j]);
            rdma->total_registrations--;
        }
        g_free(block->pmr);
        block->pmr = NULL;
    }

    if (block->mr) {
        ibv_dereg_mr(block->mr);
        rdma->total_registrations--;
        block->mr = NULL;
    }

    g_free(block->transit_bitmap);
    block->transit_bitmap = NULL;

    g_free(block->unregister_bitmap);
    block->unregister_bitmap = NULL;

    g_free(block->remote_keys);
    block->remote_keys = NULL;

    g_free(block->block_name);
    block->block_name = NULL;

    if (rdma->blockmap) {
        for (x = 0; x < local->nb_blocks; x++) {
            g_hash_table_remove(rdma->blockmap,
                                (void *)(uintptr_t)old[x].offset);
        }
    }

    if (local->nb_blocks > 1) {

        local->block = g_malloc0(sizeof(RDMALocalBlock) *
                                    (local->nb_blocks - 1));

        if (block->index) {
            memcpy(local->block, old, sizeof(RDMALocalBlock) * block->index);
        }

        if (block->index < (local->nb_blocks - 1)) {
            memcpy(local->block + block->index, old + (block->index + 1),
                sizeof(RDMALocalBlock) *
                    (local->nb_blocks - (block->index + 1)));
        }
    } else {
        assert(block == local->block);
        local->block = NULL;
    }

    trace_rdma_delete_block(block, (uintptr_t)block->local_host_addr,
                           block->offset, block->length,
                            (uintptr_t)(block->local_host_addr + block->length),
                           BITS_TO_LONGS(block->nb_chunks) *
                               sizeof(unsigned long) * 8, block->nb_chunks);

    g_free(old);

    local->nb_blocks--;

    if (local->nb_blocks && rdma->blockmap) {
        for (x = 0; x < local->nb_blocks; x++) {
            g_hash_table_insert(rdma->blockmap,
                                (void *)(uintptr_t)local->block[x].offset,
                                &local->block[x]);
        }
    }

    return 0;
}
