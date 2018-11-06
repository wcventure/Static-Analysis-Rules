void gen_intermediate_code(CPUOpenRISCState *env, struct TranslationBlock *tb)
{
    OpenRISCCPU *cpu = openrisc_env_get_cpu(env);
    CPUState *cs = CPU(cpu);
    struct DisasContext ctx, *dc = &ctx;
    uint32_t pc_start;
    uint32_t next_page_start;
    int num_insns;
    int max_insns;

    pc_start = tb->pc;
    dc->tb = tb;

    dc->is_jmp = DISAS_NEXT;
    dc->ppc = pc_start;
    dc->pc = pc_start;
    dc->flags = cpu->env.cpucfgr;
    dc->mem_idx = cpu_mmu_index(&cpu->env, false);
    dc->synced_flags = dc->tb_flags = tb->flags;
    dc->delayed_branch = !!(dc->tb_flags & D_FLAG);
    dc->singlestep_enabled = cs->singlestep_enabled;

    next_page_start = (pc_start & TARGET_PAGE_MASK) + TARGET_PAGE_SIZE;
    num_insns = 0;
    max_insns = tb->cflags & CF_COUNT_MASK;

    if (max_insns == 0) {
        max_insns = CF_COUNT_MASK;
    }
    if (max_insns > TCG_MAX_INSNS) {
        max_insns = TCG_MAX_INSNS;
    }

    if (qemu_loglevel_mask(CPU_LOG_TB_IN_ASM)
        && qemu_log_in_addr_range(pc_start)) {
        qemu_log_lock();
        qemu_log("----------------\n");
        qemu_log("IN: %s\n", lookup_symbol(pc_start));
    }

    gen_tb_start(tb);

    do {
        tcg_gen_insn_start(dc->pc);
        num_insns++;

        if (unlikely(cpu_breakpoint_test(cs, dc->pc, BP_ANY))) {
            tcg_gen_movi_tl(cpu_pc, dc->pc);
            gen_exception(dc, EXCP_DEBUG);
            dc->is_jmp = DISAS_UPDATE;
            /
            dc->pc += 4;
            break;
        }

        if (num_insns == max_insns && (tb->cflags & CF_LAST_IO)) {
            gen_io_start();
        }
        dc->ppc = dc->pc - 4;
        dc->npc = dc->pc + 4;
        tcg_gen_movi_tl(cpu_ppc, dc->ppc);
        tcg_gen_movi_tl(cpu_npc, dc->npc);
        disas_openrisc_insn(dc, cpu);
        dc->pc = dc->npc;
        /
        if (dc->delayed_branch) {
            dc->delayed_branch--;
            if (!dc->delayed_branch) {
                dc->tb_flags &= ~D_FLAG;
                gen_sync_flags(dc);
                tcg_gen_mov_tl(cpu_pc, jmp_pc);
                tcg_gen_mov_tl(cpu_npc, jmp_pc);
                tcg_gen_movi_tl(jmp_pc, 0);
                tcg_gen_exit_tb(0);
                dc->is_jmp = DISAS_JUMP;
                break;
            }
        }
    } while (!dc->is_jmp
             && !tcg_op_buf_full()
             && !cs->singlestep_enabled
             && !singlestep
             && (dc->pc < next_page_start)
             && num_insns < max_insns);

    if (tb->cflags & CF_LAST_IO) {
        gen_io_end();
    }
    if (dc->is_jmp == DISAS_NEXT) {
        dc->is_jmp = DISAS_UPDATE;
        tcg_gen_movi_tl(cpu_pc, dc->pc);
    }
    if (unlikely(cs->singlestep_enabled)) {
        if (dc->is_jmp == DISAS_NEXT) {
            tcg_gen_movi_tl(cpu_pc, dc->pc);
        }
        gen_exception(dc, EXCP_DEBUG);
    } else {
        switch (dc->is_jmp) {
        case DISAS_NEXT:
            gen_goto_tb(dc, 0, dc->pc);
            break;
        default:
        case DISAS_JUMP:
            break;
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

    if (qemu_loglevel_mask(CPU_LOG_TB_IN_ASM)
        && qemu_log_in_addr_range(pc_start)) {
        log_target_disas(cs, pc_start, tb->size, 0);
        qemu_log("\n");
        qemu_log_unlock();
    }
}
