static void tcg_out_movi_int(TCGContext *s, TCGType type, TCGReg ret,
                             tcg_target_long arg, bool in_prologue)
{
    intptr_t tb_diff;
    tcg_target_long tmp;
    int shift;

    tcg_debug_assert(TCG_TARGET_REG_BITS == 64 || type == TCG_TYPE_I32);

    if (TCG_TARGET_REG_BITS == 64 && type == TCG_TYPE_I32) {
        arg = (int32_t)arg;
    }

    /
    if (tcg_out_movi_one(s, ret, arg)) {
        return;
    }

    /
    tb_diff = arg - (intptr_t)s->code_gen_ptr;
    if (!in_prologue && USE_REG_TB && tb_diff == (int16_t)tb_diff) {
        tcg_out32(s, ADDI | TAI(ret, TCG_REG_TB, tb_diff));
        return;
    }

    /
    if (TCG_TARGET_REG_BITS == 32 || arg == (int32_t)arg) {
        tcg_out32(s, ADDIS | TAI(ret, 0, arg >> 16));
        tcg_out32(s, ORI | SAI(ret, ret, arg));
        return;
    }
    if (arg == (uint32_t)arg && !(arg & 0x8000)) {
        tcg_out32(s, ADDI | TAI(ret, 0, arg));
        tcg_out32(s, ORIS | SAI(ret, ret, arg >> 16));
        return;
    }

    /
    if (arg > 0 && (arg & 0x8000)) {
        tmp = arg | 0x7fff;
        if ((tmp & (tmp + 1)) == 0) {
            int mb = clz64(tmp + 1) + 1;
            tcg_out32(s, ADDI | TAI(ret, 0, arg));
            tcg_out_rld(s, RLDICL, ret, ret, 0, mb);
            return;
        }
    }

    /
    shift = ctz64(arg);
    tmp = arg >> shift;
    if (tmp == (int16_t)tmp) {
        tcg_out32(s, ADDI | TAI(ret, 0, tmp));
        tcg_out_shli64(s, ret, ret, shift);
        return;
    }
    shift = clz64(arg);
    if (tcg_out_movi_one(s, ret, arg << shift)) {
        tcg_out_shri64(s, ret, ret, shift);
        return;
    }

    /
    if (!in_prologue && USE_REG_TB && tb_diff == (int32_t)tb_diff) {
        tcg_out_mem_long(s, ADDI, ADD, ret, TCG_REG_TB, tb_diff);
        return;
    }

    /
    if (!in_prologue && USE_REG_TB) {
        new_pool_label(s, arg, R_PPC_ADDR16, s->code_ptr,
                       -(intptr_t)s->code_gen_ptr);
        tcg_out32(s, LD | TAI(ret, TCG_REG_TB, 0));
        return;
    }

    tmp = arg >> 31 >> 1;
    tcg_out_movi(s, TCG_TYPE_I32, ret, tmp);
    if (tmp) {
        tcg_out_shli64(s, ret, ret, 32);
    }
    if (arg & 0xffff0000) {
        tcg_out32(s, ORIS | SAI(ret, ret, arg >> 16));
    }
    if (arg & 0xffff) {
        tcg_out32(s, ORI | SAI(ret, ret, arg));
    }
}
