static inline void code_gen_alloc(size_t tb_size)
{
    tcg_ctx.code_gen_buffer_size = size_code_gen_buffer(tb_size);
    tcg_ctx.code_gen_buffer = alloc_code_gen_buffer();
    if (tcg_ctx.code_gen_buffer == NULL) {
        fprintf(stderr, "Could not allocate dynamic translator buffer\n");
        exit(1);
    }

    qemu_madvise(tcg_ctx.code_gen_buffer, tcg_ctx.code_gen_buffer_size,
                 QEMU_MADV_HUGEPAGE);

    /
    tcg_ctx.code_gen_max_blocks
        = tcg_ctx.code_gen_buffer_size / CODE_GEN_AVG_BLOCK_SIZE;
    tcg_ctx.tb_ctx.tbs = g_new(TranslationBlock, tcg_ctx.code_gen_max_blocks);

    qemu_mutex_init(&tcg_ctx.tb_ctx.tb_lock);
}
