static int qemu_rdma_init_ram_blocks(RDMAContext *rdma)
{
    RDMALocalBlocks *local = &rdma->local_ram_blocks;

    assert(rdma->blockmap == NULL);
    memset(local, 0, sizeof *local);
    qemu_ram_foreach_block(qemu_rdma_init_one_block, rdma);
    trace_qemu_rdma_init_ram_blocks(local->nb_blocks);
    rdma->dest_blocks = (RDMADestBlock *) g_malloc0(sizeof(RDMADestBlock) *
                        rdma->local_ram_blocks.nb_blocks);
    local->init = true;
    return 0;
}
