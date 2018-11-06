static int qemu_rdma_register_and_get_keys(RDMAContext *rdma,
        RDMALocalBlock *block, uintptr_t host_addr,
        uint32_t *lkey, uint32_t *rkey, int chunk,
        uint8_t *chunk_start, uint8_t *chunk_end)
{
    if (block->mr) {
        if (lkey) {
            *lkey = block->mr->lkey;
        }
        if (rkey) {
            *rkey = block->mr->rkey;
        }
        return 0;
    }

    /
    if (!block->pmr) {
        block->pmr = g_malloc0(block->nb_chunks * sizeof(struct ibv_mr *));
    }

    /
    if (!block->pmr[chunk]) {
        uint64_t len = chunk_end - chunk_start;

        trace_qemu_rdma_register_and_get_keys(len, chunk_start);

        block->pmr[chunk] = ibv_reg_mr(rdma->pd,
                chunk_start, len,
                (rkey ? (IBV_ACCESS_LOCAL_WRITE |
                        IBV_ACCESS_REMOTE_WRITE) : 0));

        if (!block->pmr[chunk]) {
            perror("Failed to register chunk!");
            fprintf(stderr, "Chunk details: block: %d chunk index %d"
                            " start %" PRIuPTR " end %" PRIuPTR
                            " host %" PRIuPTR
                            " local %" PRIuPTR " registrations: %d\n",
                            block->index, chunk, (uintptr_t)chunk_start,
                            (uintptr_t)chunk_end, host_addr,
                            (uintptr_t)block->local_host_addr,
                            rdma->total_registrations);
            return -1;
        }
        rdma->total_registrations++;
    }

    if (lkey) {
        *lkey = block->pmr[chunk]->lkey;
    }
    if (rkey) {
        *rkey = block->pmr[chunk]->rkey;
    }
    return 0;
}
