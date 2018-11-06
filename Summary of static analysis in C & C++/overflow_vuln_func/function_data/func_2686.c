int tcg_gen_code(TCGContext *s, TranslationBlock *tb)
{
#ifdef CONFIG_PROFILER
    TCGProfile *prof = &s->prof;
#endif
    int i, oi, oi_next, num_insns;

#ifdef CONFIG_PROFILER
    {
        int n;

        n = s->gen_op_buf[0].prev + 1;
        atomic_set(&prof->op_count, prof->op_count + n);
        if (n > prof->op_count_max) {
            atomic_set(&prof->op_count_max, n);
        }

        n = s->nb_temps;
        atomic_set(&prof->temp_count, prof->temp_count + n);
        if (n > prof->temp_count_max) {
            atomic_set(&prof->temp_count_max, n);
        }
    }
#endif

#ifdef DEBUG_DISAS
    if (unlikely(qemu_loglevel_mask(CPU_LOG_TB_OP)
                 && qemu_log_in_addr_range(tb->pc))) {
        qemu_log_lock();
        qemu_log("OP:\n");
        tcg_dump_ops(s);
        qemu_log("\n");
        qemu_log_unlock();
    }
#endif

#ifdef CONFIG_PROFILER
    atomic_set(&prof->opt_time, prof->opt_time - profile_getclock());
#endif

#ifdef USE_TCG_OPTIMIZATIONS
    tcg_optimize(s);
#endif

#ifdef CONFIG_PROFILER
    atomic_set(&prof->opt_time, prof->opt_time + profile_getclock());
    atomic_set(&prof->la_time, prof->la_time - profile_getclock());
#endif

    liveness_pass_1(s);

    if (s->nb_indirects > 0) {
#ifdef DEBUG_DISAS
        if (unlikely(qemu_loglevel_mask(CPU_LOG_TB_OP_IND)
                     && qemu_log_in_addr_range(tb->pc))) {
            qemu_log_lock();
            qemu_log("OP before indirect lowering:\n");
            tcg_dump_ops(s);
            qemu_log("\n");
            qemu_log_unlock();
        }
#endif
        /
        if (liveness_pass_2(s)) {
            /
            liveness_pass_1(s);
        }
    }

#ifdef CONFIG_PROFILER
    atomic_set(&prof->la_time, prof->la_time + profile_getclock());
#endif

#ifdef DEBUG_DISAS
    if (unlikely(qemu_loglevel_mask(CPU_LOG_TB_OP_OPT)
                 && qemu_log_in_addr_range(tb->pc))) {
        qemu_log_lock();
        qemu_log("OP after optimization and liveness analysis:\n");
        tcg_dump_ops(s);
        qemu_log("\n");
        qemu_log_unlock();
    }
#endif

    tcg_reg_alloc_start(s);

    s->code_buf = tb->tc.ptr;
    s->code_ptr = tb->tc.ptr;

#ifdef TCG_TARGET_NEED_LDST_LABELS
    s->ldst_labels = NULL;
#endif
#ifdef TCG_TARGET_NEED_POOL_LABELS
    s->pool_labels = NULL;
#endif

    num_insns = -1;
    for (oi = s->gen_op_buf[0].next; oi != 0; oi = oi_next) {
        TCGOp * const op = &s->gen_op_buf[oi];
        TCGOpcode opc = op->opc;

        oi_next = op->next;
#ifdef CONFIG_PROFILER
        atomic_set(&prof->table_op_count[opc], prof->table_op_count[opc] + 1);
#endif

        switch (opc) {
        case INDEX_op_mov_i32:
        case INDEX_op_mov_i64:
            tcg_reg_alloc_mov(s, op);
            break;
        case INDEX_op_movi_i32:
        case INDEX_op_movi_i64:
            tcg_reg_alloc_movi(s, op);
            break;
        case INDEX_op_insn_start:
            if (num_insns >= 0) {
                s->gen_insn_end_off[num_insns] = tcg_current_code_size(s);
            }
            num_insns++;
            for (i = 0; i < TARGET_INSN_START_WORDS; ++i) {
                target_ulong a;
#if TARGET_LONG_BITS > TCG_TARGET_REG_BITS
                a = deposit64(op->args[i * 2], 32, 32, op->args[i * 2 + 1]);
#else
                a = op->args[i];
#endif
                s->gen_insn_data[num_insns][i] = a;
            }
            break;
        case INDEX_op_discard:
            temp_dead(s, arg_temp(op->args[0]));
            break;
        case INDEX_op_set_label:
            tcg_reg_alloc_bb_end(s, s->reserved_regs);
            tcg_out_label(s, arg_label(op->args[0]), s->code_ptr);
            break;
        case INDEX_op_call:
            tcg_reg_alloc_call(s, op);
            break;
        default:
            /
            tcg_debug_assert(tcg_op_supported(opc));
            /
            tcg_reg_alloc_op(s, op);
            break;
        }
#ifdef CONFIG_DEBUG_TCG
        check_regs(s);
#endif
        /
        if (unlikely((void *)s->code_ptr > s->code_gen_highwater)) {
            return -1;
        }
    }
    tcg_debug_assert(num_insns >= 0);
    s->gen_insn_end_off[num_insns] = tcg_current_code_size(s);

    /
#ifdef TCG_TARGET_NEED_LDST_LABELS
    if (!tcg_out_ldst_finalize(s)) {
        return -1;
    }
#endif
#ifdef TCG_TARGET_NEED_POOL_LABELS
    if (!tcg_out_pool_finalize(s)) {
        return -1;
    }
#endif

    /
    flush_icache_range((uintptr_t)s->code_buf, (uintptr_t)s->code_ptr);

    return tcg_current_code_size(s);
}
