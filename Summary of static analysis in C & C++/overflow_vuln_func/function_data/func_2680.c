void tcg_dump_ops(TCGContext *s)
{
    char buf[128];
    TCGOp *op;
    int oi;

    for (oi = s->gen_op_buf[0].next; oi != 0; oi = op->next) {
        int i, k, nb_oargs, nb_iargs, nb_cargs;
        const TCGOpDef *def;
        TCGOpcode c;
        int col = 0;

        op = &s->gen_op_buf[oi];
        c = op->opc;
        def = &tcg_op_defs[c];

        if (c == INDEX_op_insn_start) {
            col += qemu_log("%s ----", oi != s->gen_op_buf[0].next ? "\n" : "");

            for (i = 0; i < TARGET_INSN_START_WORDS; ++i) {
                target_ulong a;
#if TARGET_LONG_BITS > TCG_TARGET_REG_BITS
                a = deposit64(op->args[i * 2], 32, 32, op->args[i * 2 + 1]);
#else
                a = op->args[i];
#endif
                col += qemu_log(" " TARGET_FMT_lx, a);
            }
        } else if (c == INDEX_op_call) {
            /
            nb_oargs = op->callo;
            nb_iargs = op->calli;
            nb_cargs = def->nb_cargs;

            /
            col += qemu_log(" %s %s,$0x%" TCG_PRIlx ",$%d", def->name,
                            tcg_find_helper(s, op->args[nb_oargs + nb_iargs]),
                            op->args[nb_oargs + nb_iargs + 1], nb_oargs);
            for (i = 0; i < nb_oargs; i++) {
                col += qemu_log(",%s", tcg_get_arg_str(s, buf, sizeof(buf),
                                                       op->args[i]));
            }
            for (i = 0; i < nb_iargs; i++) {
                TCGArg arg = op->args[nb_oargs + i];
                const char *t = "<dummy>";
                if (arg != TCG_CALL_DUMMY_ARG) {
                    t = tcg_get_arg_str(s, buf, sizeof(buf), arg);
                }
                col += qemu_log(",%s", t);
            }
        } else {
            col += qemu_log(" %s ", def->name);

            nb_oargs = def->nb_oargs;
            nb_iargs = def->nb_iargs;
            nb_cargs = def->nb_cargs;

            k = 0;
            for (i = 0; i < nb_oargs; i++) {
                if (k != 0) {
                    col += qemu_log(",");
                }
                col += qemu_log("%s", tcg_get_arg_str(s, buf, sizeof(buf),
                                                      op->args[k++]));
            }
            for (i = 0; i < nb_iargs; i++) {
                if (k != 0) {
                    col += qemu_log(",");
                }
                col += qemu_log("%s", tcg_get_arg_str(s, buf, sizeof(buf),
                                                      op->args[k++]));
            }
            switch (c) {
            case INDEX_op_brcond_i32:
            case INDEX_op_setcond_i32:
            case INDEX_op_movcond_i32:
            case INDEX_op_brcond2_i32:
            case INDEX_op_setcond2_i32:
            case INDEX_op_brcond_i64:
            case INDEX_op_setcond_i64:
            case INDEX_op_movcond_i64:
                if (op->args[k] < ARRAY_SIZE(cond_name)
                    && cond_name[op->args[k]]) {
                    col += qemu_log(",%s", cond_name[op->args[k++]]);
                } else {
                    col += qemu_log(",$0x%" TCG_PRIlx, op->args[k++]);
                }
                i = 1;
                break;
            case INDEX_op_qemu_ld_i32:
            case INDEX_op_qemu_st_i32:
            case INDEX_op_qemu_ld_i64:
            case INDEX_op_qemu_st_i64:
                {
                    TCGMemOpIdx oi = op->args[k++];
                    TCGMemOp op = get_memop(oi);
                    unsigned ix = get_mmuidx(oi);

                    if (op & ~(MO_AMASK | MO_BSWAP | MO_SSIZE)) {
                        col += qemu_log(",$0x%x,%u", op, ix);
                    } else {
                        const char *s_al, *s_op;
                        s_al = alignment_name[(op & MO_AMASK) >> MO_ASHIFT];
                        s_op = ldst_name[op & (MO_BSWAP | MO_SSIZE)];
                        col += qemu_log(",%s%s,%u", s_al, s_op, ix);
                    }
                    i = 1;
                }
                break;
            default:
                i = 0;
                break;
            }
            switch (c) {
            case INDEX_op_set_label:
            case INDEX_op_br:
            case INDEX_op_brcond_i32:
            case INDEX_op_brcond_i64:
            case INDEX_op_brcond2_i32:
                col += qemu_log("%s$L%d", k ? "," : "",
                                arg_label(op->args[k])->id);
                i++, k++;
                break;
            default:
                break;
            }
            for (; i < nb_cargs; i++, k++) {
                col += qemu_log("%s$0x%" TCG_PRIlx, k ? "," : "", op->args[k]);
            }
        }
        if (op->life) {
            unsigned life = op->life;

            for (; col < 48; ++col) {
                putc(' ', qemu_logfile);
            }

            if (life & (SYNC_ARG * 3)) {
                qemu_log("  sync:");
                for (i = 0; i < 2; ++i) {
                    if (life & (SYNC_ARG << i)) {
                        qemu_log(" %d", i);
                    }
                }
            }
            life /= DEAD_ARG;
            if (life) {
                qemu_log("  dead:");
                for (i = 0; life; ++i, life >>= 1) {
                    if (life & 1) {
                        qemu_log(" %d", i);
                    }
                }
            }
        }
        qemu_log("\n");
    }
}
