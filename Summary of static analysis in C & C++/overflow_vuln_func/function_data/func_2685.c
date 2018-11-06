static bool liveness_pass_2(TCGContext *s)
{
    int nb_globals = s->nb_globals;
    int nb_temps, i, oi, oi_next;
    bool changes = false;

    /
    for (i = 0; i < nb_globals; ++i) {
        TCGTemp *its = &s->temps[i];
        if (its->indirect_reg) {
            TCGTemp *dts = tcg_temp_alloc(s);
            dts->type = its->type;
            dts->base_type = its->base_type;
            its->state_ptr = dts;
        } else {
            its->state_ptr = NULL;
        }
        /
        its->state = TS_DEAD;
    }
    for (nb_temps = s->nb_temps; i < nb_temps; ++i) {
        TCGTemp *its = &s->temps[i];
        its->state_ptr = NULL;
        its->state = TS_DEAD;
    }

    for (oi = s->gen_op_buf[0].next; oi != 0; oi = oi_next) {
        TCGOp *op = &s->gen_op_buf[oi];
        TCGOpcode opc = op->opc;
        const TCGOpDef *def = &tcg_op_defs[opc];
        TCGLifeData arg_life = op->life;
        int nb_iargs, nb_oargs, call_flags;
        TCGTemp *arg_ts, *dir_ts;

        oi_next = op->next;

        if (opc == INDEX_op_call) {
            nb_oargs = op->callo;
            nb_iargs = op->calli;
            call_flags = op->args[nb_oargs + nb_iargs + 1];
        } else {
            nb_iargs = def->nb_iargs;
            nb_oargs = def->nb_oargs;

            /
            if (def->flags & TCG_OPF_BB_END) {
                /
                call_flags = 0;
            } else if (def->flags & TCG_OPF_SIDE_EFFECTS) {
                /
                call_flags = TCG_CALL_NO_WRITE_GLOBALS;
            } else {
                /
                call_flags = (TCG_CALL_NO_READ_GLOBALS |
                              TCG_CALL_NO_WRITE_GLOBALS);
            }
        }

        /
        for (i = nb_oargs; i < nb_iargs + nb_oargs; i++) {
            arg_ts = arg_temp(op->args[i]);
            if (arg_ts) {
                dir_ts = arg_ts->state_ptr;
                if (dir_ts && arg_ts->state == TS_DEAD) {
                    TCGOpcode lopc = (arg_ts->type == TCG_TYPE_I32
                                      ? INDEX_op_ld_i32
                                      : INDEX_op_ld_i64);
                    TCGOp *lop = tcg_op_insert_before(s, op, lopc, 3);

                    lop->args[0] = temp_arg(dir_ts);
                    lop->args[1] = temp_arg(arg_ts->mem_base);
                    lop->args[2] = arg_ts->mem_offset;

                    /
                    arg_ts->state = TS_MEM;
                }
            }
        }

        /
        for (i = nb_oargs; i < nb_iargs + nb_oargs; i++) {
            arg_ts = arg_temp(op->args[i]);
            if (arg_ts) {
                dir_ts = arg_ts->state_ptr;
                if (dir_ts) {
                    op->args[i] = temp_arg(dir_ts);
                    changes = true;
                    if (IS_DEAD_ARG(i)) {
                        arg_ts->state = TS_DEAD;
                    }
                }
            }
        }

        /
        if (call_flags & TCG_CALL_NO_READ_GLOBALS) {
            /
        } else if (call_flags & TCG_CALL_NO_WRITE_GLOBALS) {
            for (i = 0; i < nb_globals; ++i) {
                /
                arg_ts = &s->temps[i];
                tcg_debug_assert(arg_ts->state_ptr == 0
                                 || arg_ts->state != 0);
            }
        } else {
            for (i = 0; i < nb_globals; ++i) {
                /
                arg_ts = &s->temps[i];
                tcg_debug_assert(arg_ts->state_ptr == 0
                                 || arg_ts->state == TS_DEAD);
            }
        }

        /
        for (i = 0; i < nb_oargs; i++) {
            arg_ts = arg_temp(op->args[i]);
            dir_ts = arg_ts->state_ptr;
            if (!dir_ts) {
                continue;
            }
            op->args[i] = temp_arg(dir_ts);
            changes = true;

            /
            arg_ts->state = 0;

            /
            if (NEED_SYNC_ARG(i)) {
                TCGOpcode sopc = (arg_ts->type == TCG_TYPE_I32
                                  ? INDEX_op_st_i32
                                  : INDEX_op_st_i64);
                TCGOp *sop = tcg_op_insert_after(s, op, sopc, 3);

                sop->args[0] = temp_arg(dir_ts);
                sop->args[1] = temp_arg(arg_ts->mem_base);
                sop->args[2] = arg_ts->mem_offset;

                arg_ts->state = TS_MEM;
            }
            /
            if (IS_DEAD_ARG(i)) {
                arg_ts->state = TS_DEAD;
            }
        }
    }

    return changes;
}
