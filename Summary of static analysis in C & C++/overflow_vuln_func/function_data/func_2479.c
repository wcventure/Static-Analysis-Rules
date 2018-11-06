static void tb_gen_code(CPUState *env,
                        target_ulong pc, target_ulong cs_base, int flags,
                        int cflags)
{
    TranslationBlock *tb;
    uint8_t *tc_ptr;
    target_ulong phys_pc, phys_page2, virt_page2;
    int code_gen_size;

    phys_pc = get_phys_addr_code(env, pc);
    tb = tb_alloc(pc);
    if (!tb) {
        /
        tb_flush(env);
        /
        tb = tb_alloc(pc);
    }
    tc_ptr = code_gen_ptr;
    tb->tc_ptr = tc_ptr;
    tb->cs_base = cs_base;
    tb->flags = flags;
    tb->cflags = cflags;
    cpu_gen_code(env, tb, CODE_GEN_MAX_SIZE, &code_gen_size);
    code_gen_ptr = (void *)(((unsigned long)code_gen_ptr + code_gen_size + CODE_GEN_ALIGN - 1) & ~(CODE_GEN_ALIGN - 1));

    /
    virt_page2 = (pc + tb->size - 1) & TARGET_PAGE_MASK;
    phys_page2 = -1;
    if ((pc & TARGET_PAGE_MASK) != virt_page2) {
        phys_page2 = get_phys_addr_code(env, virt_page2);
    }
    tb_link_phys(tb, phys_pc, phys_page2);
}
