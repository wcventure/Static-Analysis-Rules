static inline
void gen_intermediate_code_internal(XtensaCPU *cpu,
                                    TranslationBlock *tb, bool search_pc)
{
    CPUState *cs = CPU(cpu);
    CPUXtensaState *env = &cpu->env;
    DisasContext dc;
    int insn_count = 0;
    int j, lj = -1;
    uint16_t *gen_opc_end = tcg_ctx.gen_opc_buf + OPC_MAX_SIZE;
    int max_insns = tb->cflags & CF_COUNT_MASK;
    uint32_t pc_start = tb->pc;
    uint32_t next_page_start =
        (pc_start & TARGET_PAGE_MASK) + TARGET_PAGE_SIZE;

    if (max_insns == 0) {
        max_insns = CF_COUNT_MASK;
    }

    dc.config = env->config;
    dc.singlestep_enabled = cs->singlestep_enabled;
    dc.tb = tb;
    dc.pc = pc_start;
    dc.ring = tb->flags & XTENSA_TBFLAG_RING_MASK;
    dc.cring = (tb->flags & XTENSA_TBFLAG_EXCM) ? 0 : dc.ring;
    dc.lbeg = env->sregs[LBEG];
    dc.lend = env->sregs[LEND];
    dc.is_jmp = DISAS_NEXT;
    dc.ccount_delta = 0;
    dc.debug = tb->flags & XTENSA_TBFLAG_DEBUG;
    dc.icount = tb->flags & XTENSA_TBFLAG_ICOUNT;
    dc.cpenable = (tb->flags & XTENSA_TBFLAG_CPENABLE_MASK) >>
        XTENSA_TBFLAG_CPENABLE_SHIFT;

    init_litbase(&dc);
    init_sar_tracker(&dc);
    reset_used_window(&dc);
    if (dc.icount) {
        dc.next_icount = tcg_temp_local_new_i32();
    }

    gen_tb_start();

    if (tb->flags & XTENSA_TBFLAG_EXCEPTION) {
        tcg_gen_movi_i32(cpu_pc, dc.pc);
        gen_exception(&dc, EXCP_DEBUG);
    }

    do {
        check_breakpoint(env, &dc);

        if (search_pc) {
            j = tcg_ctx.gen_opc_ptr - tcg_ctx.gen_opc_buf;
            if (lj < j) {
                lj++;
                while (lj < j) {
                    tcg_ctx.gen_opc_instr_start[lj++] = 0;
                }
            }
            tcg_ctx.gen_opc_pc[lj] = dc.pc;
            tcg_ctx.gen_opc_instr_start[lj] = 1;
            tcg_ctx.gen_opc_icount[lj] = insn_count;
        }

        if (unlikely(qemu_loglevel_mask(CPU_LOG_TB_OP | CPU_LOG_TB_OP_OPT))) {
            tcg_gen_debug_insn_start(dc.pc);
        }

        ++dc.ccount_delta;

        if (insn_count + 1 == max_insns && (tb->cflags & CF_LAST_IO)) {
            gen_io_start();
        }

        if (dc.icount) {
            int label = gen_new_label();

            tcg_gen_addi_i32(dc.next_icount, cpu_SR[ICOUNT], 1);
            tcg_gen_brcondi_i32(TCG_COND_NE, dc.next_icount, 0, label);
            tcg_gen_mov_i32(dc.next_icount, cpu_SR[ICOUNT]);
            if (dc.debug) {
                gen_debug_exception(&dc, DEBUGCAUSE_IC);
            }
            gen_set_label(label);
        }

        if (dc.debug) {
            gen_ibreak_check(env, &dc);
        }

        disas_xtensa_insn(env, &dc);
        ++insn_count;
        if (dc.icount) {
            tcg_gen_mov_i32(cpu_SR[ICOUNT], dc.next_icount);
        }
        if (cs->singlestep_enabled) {
            tcg_gen_movi_i32(cpu_pc, dc.pc);
            gen_exception(&dc, EXCP_DEBUG);
            break;
        }
    } while (dc.is_jmp == DISAS_NEXT &&
            insn_count < max_insns &&
            dc.pc < next_page_start &&
            dc.pc + xtensa_insn_len(env, &dc) <= next_page_start &&
            tcg_ctx.gen_opc_ptr < gen_opc_end);

    reset_litbase(&dc);
    reset_sar_tracker(&dc);
    if (dc.icount) {
        tcg_temp_free(dc.next_icount);
    }

    if (tb->cflags & CF_LAST_IO) {
        gen_io_end();
    }

    if (dc.is_jmp == DISAS_NEXT) {
        gen_jumpi(&dc, dc.pc, 0);
    }
    gen_tb_end(tb, insn_count);
    *tcg_ctx.gen_opc_ptr = INDEX_op_end;

#ifdef DEBUG_DISAS
    if (qemu_loglevel_mask(CPU_LOG_TB_IN_ASM)) {
        qemu_log("----------------\n");
        qemu_log("IN: %s\n", lookup_symbol(pc_start));
        log_target_disas(env, pc_start, dc.pc - pc_start, 0);
        qemu_log("\n");
    }
#endif
    if (search_pc) {
        j = tcg_ctx.gen_opc_ptr - tcg_ctx.gen_opc_buf;
        memset(tcg_ctx.gen_opc_instr_start + lj + 1, 0,
                (j - lj) * sizeof(tcg_ctx.gen_opc_instr_start[0]));
    } else {
        tb->size = dc.pc - pc_start;
        tb->icount = insn_count;
    }
}
