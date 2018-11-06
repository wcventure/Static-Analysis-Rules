void gen_intermediate_code(CPUState *cs, struct TranslationBlock *tb)
{
    CPULM32State *env = cs->env_ptr;
    LM32CPU *cpu = lm32_env_get_cpu(env);
    struct DisasContext ctx, *dc = &ctx;
    uint32_t pc_start;
    uint32_t next_page_start;
    int num_insns;
    int max_insns;

    pc_start = tb->pc;
    dc->features = cpu->features;
    dc->num_breakpoints = cpu->num_breakpoints;
    dc->num_watchpoints = cpu->num_watchpoints;
    dc->tb = tb;

    dc->is_jmp = DISAS_NEXT;
    dc->pc = pc_start;
    dc->singlestep_enabled = cs->singlestep_enabled;

    if (pc_start & 3) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "unaligned PC=%x. Ignoring lowest bits.\n", pc_start);
        pc_start &= ~3;
    }

    next_page_start = (pc_start & TARGET_PAGE_MASK) + TARGET_PAGE_SIZE;
    num_insns = 0;
    max_insns = tb_cflags(tb) & CF_COUNT_MASK;
    if (max_insns == 0) {
        max_insns = CF_COUNT_MASK;
    }
    if (max_insns > TCG_MAX_INSNS) {
        max_insns = TCG_MAX_INSNS;
    }

    gen_tb_start(tb);
    do {
        tcg_gen_insn_start(dc->pc);
        num_insns++;

        if (unlikely(cpu_breakpoint_test(cs, dc->pc, BP_ANY))) {
            tcg_gen_movi_tl(cpu_pc, dc->pc);
            t_gen_raise_exception(dc, EXCP_DEBUG);
            dc->is_jmp = DISAS_UPDATE;
            /
            dc->pc += 4;
            break;
        }

        /
        LOG_DIS("%8.8x:\t", dc->pc);

        if (num_insns == max_insns && (tb_cflags(tb) & CF_LAST_IO)) {
            gen_io_start();
        }

        decode(dc, cpu_ldl_code(env, dc->pc));
        dc->pc += 4;
    } while (!dc->is_jmp
         && !tcg_op_buf_full()
         && !cs->singlestep_enabled
         && !singlestep
         && (dc->pc < next_page_start)
         && num_insns < max_insns);

    if (tb_cflags(tb) & CF_LAST_IO) {
        gen_io_end();
    }

    if (unlikely(cs->singlestep_enabled)) {
        if (dc->is_jmp == DISAS_NEXT) {
            tcg_gen_movi_tl(cpu_pc, dc->pc);
        }
        t_gen_raise_exception(dc, EXCP_DEBUG);
    } else {
        switch (dc->is_jmp) {
        case DISAS_NEXT:
            gen_goto_tb(dc, 1, dc->pc);
            break;
        default:
        case DISAS_JUMP:
        case DISAS_UPDATE:
            /
            tcg_gen_exit_tb(0);
            break;
        case DISAS_TB_JUMP:
            /
            break;
        }
    }

    gen_tb_end(tb, num_insns);

    tb->size = dc->pc - pc_start;
    tb->icount = num_insns;

#ifdef DEBUG_DISAS
    if (qemu_loglevel_mask(CPU_LOG_TB_IN_ASM)
        && qemu_log_in_addr_range(pc_start)) {
        qemu_log_lock();
        qemu_log("\n");
        log_target_disas(cs, pc_start, dc->pc - pc_start);
        qemu_log("\nisize=%d osize=%d\n",
                 dc->pc - pc_start, tcg_op_buf_count());
        qemu_log_unlock();
    }
#endif
}
