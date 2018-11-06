static TranslationBlock *tb_find_slow(target_ulong pc,
                                      target_ulong cs_base,
                                      uint64_t flags)
{
    TranslationBlock *tb, **ptb1;
    int code_gen_size;
    unsigned int h;
    target_ulong phys_pc, phys_page1, phys_page2, virt_page2;
    uint8_t *tc_ptr;

    spin_lock(&tb_lock);

    tb_invalidated_flag = 0;

    regs_to_env(); /

    /
    phys_pc = get_phys_addr_code(env, pc);
    phys_page1 = phys_pc & TARGET_PAGE_MASK;
    phys_page2 = -1;
    h = tb_phys_hash_func(phys_pc);
    ptb1 = &tb_phys_hash[h];
    for(;;) {
        tb = *ptb1;
        if (!tb)
            goto not_found;
        if (tb->pc == pc &&
            tb->page_addr[0] == phys_page1 &&
            tb->cs_base == cs_base &&
            tb->flags == flags) {
            /
            if (tb->page_addr[1] != -1) {
                virt_page2 = (pc & TARGET_PAGE_MASK) +
                    TARGET_PAGE_SIZE;
                phys_page2 = get_phys_addr_code(env, virt_page2);
                if (tb->page_addr[1] == phys_page2)
                    goto found;
            } else {
                goto found;
            }
        }
        ptb1 = &tb->phys_hash_next;
    }
 not_found:
    /
    tb = tb_alloc(pc);
    if (!tb) {
        /
        tb_flush(env);
        /
        tb = tb_alloc(pc);
        /
        tb_invalidated_flag = 1;
    }
    tc_ptr = code_gen_ptr;
    tb->tc_ptr = tc_ptr;
    tb->cs_base = cs_base;
    tb->flags = flags;
    cpu_gen_code(env, tb, CODE_GEN_MAX_SIZE, &code_gen_size);
    code_gen_ptr = (void *)(((unsigned long)code_gen_ptr + code_gen_size + CODE_GEN_ALIGN - 1) & ~(CODE_GEN_ALIGN - 1));

    /
    virt_page2 = (pc + tb->size - 1) & TARGET_PAGE_MASK;
    phys_page2 = -1;
    if ((pc & TARGET_PAGE_MASK) != virt_page2) {
        phys_page2 = get_phys_addr_code(env, virt_page2);
    }
    tb_link_phys(tb, phys_pc, phys_page2);

 found:
    /
    env->tb_jmp_cache[tb_jmp_cache_hash_func(pc)] = tb;
    spin_unlock(&tb_lock);
    return tb;
}
