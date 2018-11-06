TranslationBlock *tb_gen_code(CPUState *cpu,
                              target_ulong pc, target_ulong cs_base,
                              uint32_t flags, int cflags)
{
    CPUArchState *env = cpu->env_ptr;
    TranslationBlock *tb;
    tb_page_addr_t phys_pc, phys_page2;
    target_ulong virt_page2;
    tcg_insn_unit *gen_code_buf;
    int gen_code_size, search_size;
#ifdef CONFIG_PROFILER
    int64_t ti;
#endif
    assert_memory_lock();

    phys_pc = get_page_addr_code(env, pc);
    if (use_icount && !(cflags & CF_IGNORE_ICOUNT)) {
        cflags |= CF_USE_ICOUNT;
    }

    tb = tb_alloc(pc);
    if (unlikely(!tb)) {
 buffer_overflow:
        /
        tb_flush(cpu);
        mmap_unlock();
        /
        cpu->exception_index = EXCP_INTERRUPT;
        cpu_loop_exit(cpu);
    }

    gen_code_buf = tcg_ctx.code_gen_ptr;
    tb->tc_ptr = gen_code_buf;
    tb->pc = pc;
    tb->cs_base = cs_base;
    tb->flags = flags;
    tb->cflags = cflags;
    tb->trace_vcpu_dstate = *cpu->trace_dstate;
    tb->invalid = false;

#ifdef CONFIG_PROFILER
    tcg_ctx.tb_count1++; /
    ti = profile_getclock();
#endif

    tcg_func_start(&tcg_ctx);

    tcg_ctx.cpu = ENV_GET_CPU(env);
    gen_intermediate_code(cpu, tb);
    tcg_ctx.cpu = NULL;

    trace_translate_block(tb, tb->pc, tb->tc_ptr);

    /
    tb->jmp_reset_offset[0] = TB_JMP_RESET_OFFSET_INVALID;
    tb->jmp_reset_offset[1] = TB_JMP_RESET_OFFSET_INVALID;
    tcg_ctx.tb_jmp_reset_offset = tb->jmp_reset_offset;
    if (TCG_TARGET_HAS_direct_jump) {
        tcg_ctx.tb_jmp_insn_offset = tb->jmp_target_arg;
        tcg_ctx.tb_jmp_target_addr = NULL;
    } else {
        tcg_ctx.tb_jmp_insn_offset = NULL;
        tcg_ctx.tb_jmp_target_addr = tb->jmp_target_arg;
    }

#ifdef CONFIG_PROFILER
    tcg_ctx.tb_count++;
    tcg_ctx.interm_time += profile_getclock() - ti;
    tcg_ctx.code_time -= profile_getclock();
#endif

    /
    gen_code_size = tcg_gen_code(&tcg_ctx, tb);
    if (unlikely(gen_code_size < 0)) {
        goto buffer_overflow;
    }
    search_size = encode_search(tb, (void *)gen_code_buf + gen_code_size);
    if (unlikely(search_size < 0)) {
        goto buffer_overflow;
    }

#ifdef CONFIG_PROFILER
    tcg_ctx.code_time += profile_getclock();
    tcg_ctx.code_in_len += tb->size;
    tcg_ctx.code_out_len += gen_code_size;
    tcg_ctx.search_out_len += search_size;
#endif

#ifdef DEBUG_DISAS
    if (qemu_loglevel_mask(CPU_LOG_TB_OUT_ASM) &&
        qemu_log_in_addr_range(tb->pc)) {
        qemu_log_lock();
        qemu_log("OUT: [size=%d]\n", gen_code_size);
        if (tcg_ctx.data_gen_ptr) {
            size_t code_size = tcg_ctx.data_gen_ptr - tb->tc_ptr;
            size_t data_size = gen_code_size - code_size;
            size_t i;

            log_disas(tb->tc_ptr, code_size);

            for (i = 0; i < data_size; i += sizeof(tcg_target_ulong)) {
                if (sizeof(tcg_target_ulong) == 8) {
                    qemu_log("0x%08" PRIxPTR ":  .quad  0x%016" PRIx64 "\n",
                             (uintptr_t)tcg_ctx.data_gen_ptr + i,
                             *(uint64_t *)(tcg_ctx.data_gen_ptr + i));
                } else {
                    qemu_log("0x%08" PRIxPTR ":  .long  0x%08x\n",
                             (uintptr_t)tcg_ctx.data_gen_ptr + i,
                             *(uint32_t *)(tcg_ctx.data_gen_ptr + i));
                }
            }
        } else {
            log_disas(tb->tc_ptr, gen_code_size);
        }
        qemu_log("\n");
        qemu_log_flush();
        qemu_log_unlock();
    }
#endif

    tcg_ctx.code_gen_ptr = (void *)
        ROUND_UP((uintptr_t)gen_code_buf + gen_code_size + search_size,
                 CODE_GEN_ALIGN);

    /
    assert(((uintptr_t)tb & 3) == 0);
    tb->jmp_list_first = (uintptr_t)tb | 2;
    tb->jmp_list_next[0] = (uintptr_t)NULL;
    tb->jmp_list_next[1] = (uintptr_t)NULL;

    /
    if (tb->jmp_reset_offset[0] != TB_JMP_RESET_OFFSET_INVALID) {
        tb_reset_jump(tb, 0);
    }
    if (tb->jmp_reset_offset[1] != TB_JMP_RESET_OFFSET_INVALID) {
        tb_reset_jump(tb, 1);
    }

    /
    virt_page2 = (pc + tb->size - 1) & TARGET_PAGE_MASK;
    phys_page2 = -1;
    if ((pc & TARGET_PAGE_MASK) != virt_page2) {
        phys_page2 = get_page_addr_code(env, virt_page2);
    }
    /
    tb_link_page(tb, phys_pc, phys_page2);
    return tb;
}
