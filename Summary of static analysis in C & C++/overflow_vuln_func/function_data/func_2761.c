void tcg_exec_init(unsigned long tb_size)
{
    cpu_gen_init();
    code_gen_alloc(tb_size);
    page_init();
#if defined(CONFIG_SOFTMMU)
    /
    tcg_prologue_init(&tcg_ctx);
#endif
}
