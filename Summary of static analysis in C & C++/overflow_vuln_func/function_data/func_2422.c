void tcg_prologue_init(TCGContext *s)
{
    size_t prologue_size, total_size;
    void *buf0, *buf1;

    /
    buf0 = s->code_gen_buffer;
    s->code_ptr = buf0;
    s->code_buf = buf0;
    s->code_gen_prologue = buf0;

    /
    tcg_target_qemu_prologue(s);
    buf1 = s->code_ptr;
    flush_icache_range((uintptr_t)buf0, (uintptr_t)buf1);

    /
    prologue_size = tcg_current_code_size(s);
    s->code_gen_ptr = buf1;
    s->code_gen_buffer = buf1;
    s->code_buf = buf1;
    total_size = s->code_gen_buffer_size - prologue_size;
    s->code_gen_buffer_size = total_size;

    /
    s->code_gen_buffer_max_size = total_size - TCG_MAX_OP_SIZE * OPC_BUF_SIZE;

    tcg_register_jit(s->code_gen_buffer, total_size);

#ifdef DEBUG_DISAS
    if (qemu_loglevel_mask(CPU_LOG_TB_OUT_ASM)) {
        qemu_log("PROLOGUE: [size=%zu]\n", prologue_size);
        log_disas(buf0, prologue_size);
        qemu_log("\n");
        qemu_log_flush();
    }
#endif
}
