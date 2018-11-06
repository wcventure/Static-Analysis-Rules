int gen_intermediate_code_internal (CPUState *env, TranslationBlock *tb,
                                    int search_pc)
{
    DisasContext ctx, *ctxp = &ctx;
    opc_handler_t **table, *handler;
    target_ulong pc_start;
    uint16_t *gen_opc_end;
    int j, lj = -1;

    pc_start = tb->pc;
    gen_opc_ptr = gen_opc_buf;
    gen_opc_end = gen_opc_buf + OPC_MAX_SIZE;
    gen_opparam_ptr = gen_opparam_buf;
    nb_gen_labels = 0;
    ctx.nip = pc_start;
    ctx.tb = tb;
    ctx.exception = EXCP_NONE;
    ctx.spr_cb = env->spr_cb;
#if defined(CONFIG_USER_ONLY)
    ctx.mem_idx = msr_le;
#else
    ctx.supervisor = 1 - msr_pr;
    ctx.mem_idx = ((1 - msr_pr) << 1) | msr_le;
#endif
    ctx.fpu_enabled = msr_fp;
    ctx.singlestep_enabled = env->singlestep_enabled;
#if defined (DO_SINGLE_STEP) && 0
    /
    msr_se = 1;
#endif
    /
    while (ctx.exception == EXCP_NONE && gen_opc_ptr < gen_opc_end) {
        if (unlikely(env->nb_breakpoints > 0)) {
            for (j = 0; j < env->nb_breakpoints; j++) {
                if (env->breakpoints[j] == ctx.nip) {
                    gen_op_update_nip(ctx.nip); 
                    gen_op_debug();
                    break;
                }
            }
        }
        if (unlikely(search_pc)) {
            j = gen_opc_ptr - gen_opc_buf;
            if (lj < j) {
                lj++;
                while (lj < j)
                    gen_opc_instr_start[lj++] = 0;
                gen_opc_pc[lj] = ctx.nip;
                gen_opc_instr_start[lj] = 1;
            }
        }
#if defined PPC_DEBUG_DISAS
        if (loglevel & CPU_LOG_TB_IN_ASM) {
            fprintf(logfile, "----------------\n");
            fprintf(logfile, "nip=%08x super=%d ir=%d\n",
                    ctx.nip, 1 - msr_pr, msr_ir);
        }
#endif
        ctx.opcode = ldl_code(ctx.nip);
        if (msr_le) {
            ctx.opcode = ((ctx.opcode & 0xFF000000) >> 24) |
                ((ctx.opcode & 0x00FF0000) >> 8) |
                ((ctx.opcode & 0x0000FF00) << 8) |
                ((ctx.opcode & 0x000000FF) << 24);
        }
#if defined PPC_DEBUG_DISAS
        if (loglevel & CPU_LOG_TB_IN_ASM) {
            fprintf(logfile, "translate opcode %08x (%02x %02x %02x) (%s)\n",
                    ctx.opcode, opc1(ctx.opcode), opc2(ctx.opcode),
                    opc3(ctx.opcode), msr_le ? "little" : "big");
        }
#endif
        ctx.nip += 4;
        table = env->opcodes;
        handler = table[opc1(ctx.opcode)];
        if (is_indirect_opcode(handler)) {
            table = ind_table(handler);
            handler = table[opc2(ctx.opcode)];
            if (is_indirect_opcode(handler)) {
                table = ind_table(handler);
                handler = table[opc3(ctx.opcode)];
            }
        }
        /
        if (unlikely(handler->handler == &gen_invalid)) {
            if (loglevel > 0) {
                fprintf(logfile, "invalid/unsupported opcode: "
                        "%02x - %02x - %02x (%08x) 0x%08x %d\n",
                        opc1(ctx.opcode), opc2(ctx.opcode),
                        opc3(ctx.opcode), ctx.opcode, ctx.nip - 4, msr_ir);
            } else {
                printf("invalid/unsupported opcode: "
                       "%02x - %02x - %02x (%08x) 0x%08x %d\n",
                       opc1(ctx.opcode), opc2(ctx.opcode),
                       opc3(ctx.opcode), ctx.opcode, ctx.nip - 4, msr_ir);
            }
        } else {
            if (unlikely((ctx.opcode & handler->inval) != 0)) {
                if (loglevel > 0) {
                    fprintf(logfile, "invalid bits: %08x for opcode: "
                            "%02x -%02x - %02x (0x%08x) (0x%08x)\n",
                            ctx.opcode & handler->inval, opc1(ctx.opcode),
                            opc2(ctx.opcode), opc3(ctx.opcode),
                            ctx.opcode, ctx.nip - 4);
                } else {
                    printf("invalid bits: %08x for opcode: "
                           "%02x -%02x - %02x (0x%08x) (0x%08x)\n",
                           ctx.opcode & handler->inval, opc1(ctx.opcode),
                           opc2(ctx.opcode), opc3(ctx.opcode),
                           ctx.opcode, ctx.nip - 4);
                }
                RET_INVAL(ctxp);
                break;
            }
        }
        (*(handler->handler))(&ctx);
#if defined(DO_PPC_STATISTICS)
        handler->count++;
#endif
        /
        if (unlikely((msr_be && ctx.exception == EXCP_BRANCH) ||
                     /
                     (msr_se && (ctx.nip < 0x100 ||
                                 ctx.nip > 0xF00 ||
                                 (ctx.nip & 0xFC) != 0x04) &&
                      ctx.exception != EXCP_SYSCALL &&
                      ctx.exception != EXCP_SYSCALL_USER &&
                      ctx.exception != EXCP_TRAP))) {
            RET_EXCP(ctxp, EXCP_TRACE, 0);
        }
        /
        if (unlikely(((ctx.nip & (TARGET_PAGE_SIZE - 1)) == 0) ||
                     (env->singlestep_enabled))) {
            break;
        }
#if defined (DO_SINGLE_STEP)
        break;
#endif
    }
    if (ctx.exception == EXCP_NONE) {
        gen_goto_tb(&ctx, 0, ctx.nip);
    } else if (ctx.exception != EXCP_BRANCH) {
        gen_op_reset_T0();
        /
        gen_op_exit_tb();
    }
    *gen_opc_ptr = INDEX_op_end;
    if (unlikely(search_pc)) {
        j = gen_opc_ptr - gen_opc_buf;
        lj++;
        while (lj <= j)
            gen_opc_instr_start[lj++] = 0;
        tb->size = 0;
    } else {
        tb->size = ctx.nip - pc_start;
    }
#ifdef DEBUG_DISAS
    if (loglevel & CPU_LOG_TB_CPU) {
        fprintf(logfile, "---------------- excp: %04x\n", ctx.exception);
        cpu_dump_state(env, logfile, fprintf, 0);
    }
    if (loglevel & CPU_LOG_TB_IN_ASM) {
        int flags;
        flags = msr_le;
        fprintf(logfile, "IN: %s\n", lookup_symbol(pc_start));
        target_disas(logfile, pc_start, ctx.nip - pc_start, flags);
        fprintf(logfile, "\n");
    }
    if (loglevel & CPU_LOG_TB_OP) {
        fprintf(logfile, "OP:\n");
        dump_ops(gen_opc_buf, gen_opparam_buf);
        fprintf(logfile, "\n");
    }
#endif
    return 0;
}
