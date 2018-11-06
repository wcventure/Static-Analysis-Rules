static void disas_xtensa_insn(DisasContext *dc)
{
#define HAS_OPTION(opt) do { \
        if (!option_enabled(dc, opt)) { \
            qemu_log("Option %d is not enabled %s:%d\n", \
                    (opt), __FILE__, __LINE__); \
            goto invalid_opcode; \
        } \
    } while (0)

#define TBD() qemu_log("TBD(pc = %08x): %s:%d\n", dc->pc, __FILE__, __LINE__)
#define RESERVED() do { \
        qemu_log("RESERVED(pc = %08x, %02x%02x%02x): %s:%d\n", \
                dc->pc, b0, b1, b2, __FILE__, __LINE__); \
        goto invalid_opcode; \
    } while (0)


#ifdef TARGET_WORDS_BIGENDIAN
#define OP0 (((b0) & 0xf0) >> 4)
#define OP1 (((b2) & 0xf0) >> 4)
#define OP2 ((b2) & 0xf)
#define RRR_R ((b1) & 0xf)
#define RRR_S (((b1) & 0xf0) >> 4)
#define RRR_T ((b0) & 0xf)
#else
#define OP0 (((b0) & 0xf))
#define OP1 (((b2) & 0xf))
#define OP2 (((b2) & 0xf0) >> 4)
#define RRR_R (((b1) & 0xf0) >> 4)
#define RRR_S (((b1) & 0xf))
#define RRR_T (((b0) & 0xf0) >> 4)
#endif

#define RRRN_R RRR_R
#define RRRN_S RRR_S
#define RRRN_T RRR_T

#define RRI8_R RRR_R
#define RRI8_S RRR_S
#define RRI8_T RRR_T
#define RRI8_IMM8 (b2)
#define RRI8_IMM8_SE ((((b2) & 0x80) ? 0xffffff00 : 0) | RRI8_IMM8)

#ifdef TARGET_WORDS_BIGENDIAN
#define RI16_IMM16 (((b1) << 8) | (b2))
#else
#define RI16_IMM16 (((b2) << 8) | (b1))
#endif

#ifdef TARGET_WORDS_BIGENDIAN
#define CALL_N (((b0) & 0xc) >> 2)
#define CALL_OFFSET ((((b0) & 0x3) << 16) | ((b1) << 8) | (b2))
#else
#define CALL_N (((b0) & 0x30) >> 4)
#define CALL_OFFSET ((((b0) & 0xc0) >> 6) | ((b1) << 2) | ((b2) << 10))
#endif
#define CALL_OFFSET_SE \
    (((CALL_OFFSET & 0x20000) ? 0xfffc0000 : 0) | CALL_OFFSET)

#define CALLX_N CALL_N
#ifdef TARGET_WORDS_BIGENDIAN
#define CALLX_M ((b0) & 0x3)
#else
#define CALLX_M (((b0) & 0xc0) >> 6)
#endif
#define CALLX_S RRR_S

#define BRI12_M CALLX_M
#define BRI12_S RRR_S
#ifdef TARGET_WORDS_BIGENDIAN
#define BRI12_IMM12 ((((b1) & 0xf) << 8) | (b2))
#else
#define BRI12_IMM12 ((((b1) & 0xf0) >> 4) | ((b2) << 4))
#endif
#define BRI12_IMM12_SE (((BRI12_IMM12 & 0x800) ? 0xfffff000 : 0) | BRI12_IMM12)

#define BRI8_M BRI12_M
#define BRI8_R RRI8_R
#define BRI8_S RRI8_S
#define BRI8_IMM8 RRI8_IMM8
#define BRI8_IMM8_SE RRI8_IMM8_SE

#define RSR_SR (b1)

    uint8_t b0 = ldub_code(dc->pc);
    uint8_t b1 = ldub_code(dc->pc + 1);
    uint8_t b2 = ldub_code(dc->pc + 2);

    static const uint32_t B4CONST[] = {
        0xffffffff, 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 16, 32, 64, 128, 256
    };

    static const uint32_t B4CONSTU[] = {
        32768, 65536, 2, 3, 4, 5, 6, 7, 8, 10, 12, 16, 32, 64, 128, 256
    };

    if (OP0 >= 8) {
        dc->next_pc = dc->pc + 2;
        HAS_OPTION(XTENSA_OPTION_CODE_DENSITY);
    } else {
        dc->next_pc = dc->pc + 3;
    }

    switch (OP0) {
    case 0: /
        switch (OP1) {
        case 0: /
            switch (OP2) {
            case 0: /
                if ((RRR_R & 0xc) == 0x8) {
                    HAS_OPTION(XTENSA_OPTION_BOOLEAN);
                }

                switch (RRR_R) {
                case 0: /
                    switch (CALLX_M) {
                    case 0: /
                        gen_exception_cause(dc, ILLEGAL_INSTRUCTION_CAUSE);
                        break;

                    case 1: /
                        RESERVED();
                        break;

                    case 2: /
                        switch (CALLX_N) {
                        case 0: /
                        case 2: /
                            gen_jump(dc, cpu_R[CALLX_S]);
                            break;

                        case 1: /
                            HAS_OPTION(XTENSA_OPTION_WINDOWED_REGISTER);
                            TBD();
                            break;

                        case 3: /
                            RESERVED();
                            break;
                        }
                        break;

                    case 3: /
                        switch (CALLX_N) {
                        case 0: /
                            {
                                TCGv_i32 tmp = tcg_temp_new_i32();
                                tcg_gen_mov_i32(tmp, cpu_R[CALLX_S]);
                                tcg_gen_movi_i32(cpu_R[0], dc->next_pc);
                                gen_jump(dc, tmp);
                                tcg_temp_free(tmp);
                            }
                            break;

                        case 1: /
                        case 2: /
                        case 3: /
                            HAS_OPTION(XTENSA_OPTION_WINDOWED_REGISTER);
                            TBD();
                            break;
                        }
                        break;
                    }
                    break;

                case 1: /
                    HAS_OPTION(XTENSA_OPTION_WINDOWED_REGISTER);
                    TBD();
                    break;

                case 2: /
                    switch (RRR_T) {
                    case 0: /
                        break;

                    case 1: /
                        break;

                    case 2: /
                        break;

                    case 3: /
                        break;

                    case 8: /
                        HAS_OPTION(XTENSA_OPTION_EXCEPTION);
                        break;

                    case 12: /
                        break;

                    case 13: /
                        break;

                    case 15: /
                        break;

                    default: /
                        RESERVED();
                        break;
                    }
                    break;

                case 3: /
                    switch (RRR_T) {
                    case 0: /
                        HAS_OPTION(XTENSA_OPTION_EXCEPTION);
                        switch (RRR_S) {
                        case 0: /
                            gen_check_privilege(dc);
                            tcg_gen_andi_i32(cpu_SR[PS], cpu_SR[PS], ~PS_EXCM);
                            gen_jump(dc, cpu_SR[EPC1]);
                            break;

                        case 1: /
                            RESERVED();
                            break;

                        case 2: /
                            gen_check_privilege(dc);
                            gen_jump(dc, cpu_SR[
                                    dc->config->ndepc ? DEPC : EPC1]);
                            break;

                        case 4: /
                        case 5: /
                            HAS_OPTION(XTENSA_OPTION_WINDOWED_REGISTER);
                            TBD();
                            break;

                        default: /
                            RESERVED();
                            break;
                        }
                        break;

                    case 1: /
                        HAS_OPTION(XTENSA_OPTION_HIGH_PRIORITY_INTERRUPT);
                        TBD();
                        break;

                    case 2: /
                        TBD();
                        break;

                    default: /
                        RESERVED();
                        break;

                    }
                    break;

                case 4: /
                    HAS_OPTION(XTENSA_OPTION_EXCEPTION);
                    TBD();
                    break;

                case 5: /
                    HAS_OPTION(XTENSA_OPTION_EXCEPTION);
                    switch (RRR_S) {
                    case 0: /
                        gen_exception_cause(dc, SYSCALL_CAUSE);
                        break;

                    case 1: /
                        TBD();
                        break;

                    default:
                        RESERVED();
                        break;
                    }
                    break;

                case 6: /
                    HAS_OPTION(XTENSA_OPTION_INTERRUPT);
                    gen_check_privilege(dc);
                    tcg_gen_mov_i32(cpu_R[RRR_T], cpu_SR[PS]);
                    tcg_gen_ori_i32(cpu_SR[PS], cpu_SR[PS], RRR_S);
                    tcg_gen_andi_i32(cpu_SR[PS], cpu_SR[PS],
                            RRR_S | ~PS_INTLEVEL);
                    break;

                case 7: /
                    HAS_OPTION(XTENSA_OPTION_INTERRUPT);
                    TBD();
                    break;

                case 8: /
                    HAS_OPTION(XTENSA_OPTION_BOOLEAN);
                    TBD();
                    break;

                case 9: /
                    HAS_OPTION(XTENSA_OPTION_BOOLEAN);
                    TBD();
                    break;

                case 10: /
                    HAS_OPTION(XTENSA_OPTION_BOOLEAN);
                    TBD();
                    break;

                case 11: /
                    HAS_OPTION(XTENSA_OPTION_BOOLEAN);
                    TBD();
                    break;

                default: /
                    RESERVED();
                    break;

                }
                break;

            case 1: /
                tcg_gen_and_i32(cpu_R[RRR_R], cpu_R[RRR_S], cpu_R[RRR_T]);
                break;

            case 2: /
                tcg_gen_or_i32(cpu_R[RRR_R], cpu_R[RRR_S], cpu_R[RRR_T]);
                break;

            case 3: /
                tcg_gen_xor_i32(cpu_R[RRR_R], cpu_R[RRR_S], cpu_R[RRR_T]);
                break;

            case 4: /
                switch (RRR_R) {
                case 0: /
                    gen_right_shift_sar(dc, cpu_R[RRR_S]);
                    break;

                case 1: /
                    gen_left_shift_sar(dc, cpu_R[RRR_S]);
                    break;

                case 2: /
                    {
                        TCGv_i32 tmp = tcg_temp_new_i32();
                        tcg_gen_shli_i32(tmp, cpu_R[RRR_S], 3);
                        gen_right_shift_sar(dc, tmp);
                        tcg_temp_free(tmp);
                    }
                    break;

                case 3: /
                    {
                        TCGv_i32 tmp = tcg_temp_new_i32();
                        tcg_gen_shli_i32(tmp, cpu_R[RRR_S], 3);
                        gen_left_shift_sar(dc, tmp);
                        tcg_temp_free(tmp);
                    }
                    break;

                case 4: /
                    {
                        TCGv_i32 tmp = tcg_const_i32(
                                RRR_S | ((RRR_T & 1) << 4));
                        gen_right_shift_sar(dc, tmp);
                        tcg_temp_free(tmp);
                    }
                    break;

                case 6: /
                    TBD();
                    break;

                case 7: /
                    TBD();
                    break;

                case 8: /
                    HAS_OPTION(XTENSA_OPTION_WINDOWED_REGISTER);
                    TBD();
                    break;

                case 14: /
                    HAS_OPTION(XTENSA_OPTION_MISC_OP);
                    gen_helper_nsa(cpu_R[RRR_T], cpu_R[RRR_S]);
                    break;

                case 15: /
                    HAS_OPTION(XTENSA_OPTION_MISC_OP);
                    gen_helper_nsau(cpu_R[RRR_T], cpu_R[RRR_S]);
                    break;

                default: /
                    RESERVED();
                    break;
                }
                break;

            case 5: /
                TBD();
                break;

            case 6: /
                switch (RRR_S) {
                case 0: /
                    tcg_gen_neg_i32(cpu_R[RRR_R], cpu_R[RRR_T]);
                    break;

                case 1: /
                    {
                        int label = gen_new_label();
                        tcg_gen_mov_i32(cpu_R[RRR_R], cpu_R[RRR_T]);
                        tcg_gen_brcondi_i32(
                                TCG_COND_GE, cpu_R[RRR_R], 0, label);
                        tcg_gen_neg_i32(cpu_R[RRR_R], cpu_R[RRR_T]);
                        gen_set_label(label);
                    }
                    break;

                default: /
                    RESERVED();
                    break;
                }
                break;

            case 7: /
                RESERVED();
                break;

            case 8: /
                tcg_gen_add_i32(cpu_R[RRR_R], cpu_R[RRR_S], cpu_R[RRR_T]);
                break;

            case 9: /
            case 10:
            case 11:
                {
                    TCGv_i32 tmp = tcg_temp_new_i32();
                    tcg_gen_shli_i32(tmp, cpu_R[RRR_S], OP2 - 8);
                    tcg_gen_add_i32(cpu_R[RRR_R], tmp, cpu_R[RRR_T]);
                    tcg_temp_free(tmp);
                }
                break;

            case 12: /
                tcg_gen_sub_i32(cpu_R[RRR_R], cpu_R[RRR_S], cpu_R[RRR_T]);
                break;

            case 13: /
            case 14:
            case 15:
                {
                    TCGv_i32 tmp = tcg_temp_new_i32();
                    tcg_gen_shli_i32(tmp, cpu_R[RRR_S], OP2 - 12);
                    tcg_gen_sub_i32(cpu_R[RRR_R], tmp, cpu_R[RRR_T]);
                    tcg_temp_free(tmp);
                }
                break;
            }
            break;

        case 1: /
            switch (OP2) {
            case 0: /
            case 1:
                tcg_gen_shli_i32(cpu_R[RRR_R], cpu_R[RRR_S],
                        32 - (RRR_T | ((OP2 & 1) << 4)));
                break;

            case 2: /
            case 3:
                tcg_gen_sari_i32(cpu_R[RRR_R], cpu_R[RRR_T],
                        RRR_S | ((OP2 & 1) << 4));
                break;

            case 4: /
                tcg_gen_shri_i32(cpu_R[RRR_R], cpu_R[RRR_T], RRR_S);
                break;

            case 6: /
                {
                    TCGv_i32 tmp = tcg_temp_new_i32();
                    if (RSR_SR >= 64) {
                        gen_check_privilege(dc);
                    }
                    tcg_gen_mov_i32(tmp, cpu_R[RRR_T]);
                    gen_rsr(dc, cpu_R[RRR_T], RSR_SR);
                    gen_wsr(dc, RSR_SR, tmp);
                    tcg_temp_free(tmp);
                    if (!sregnames[RSR_SR]) {
                        TBD();
                    }
                }
                break;

                /
#define gen_shift_reg(cmd, reg) do { \
                    TCGv_i64 tmp = tcg_temp_new_i64(); \
                    tcg_gen_extu_i32_i64(tmp, reg); \
                    tcg_gen_##cmd##_i64(v, v, tmp); \
                    tcg_gen_trunc_i64_i32(cpu_R[RRR_R], v); \
                    tcg_temp_free_i64(v); \
                    tcg_temp_free_i64(tmp); \
                } while (0)

#define gen_shift(cmd) gen_shift_reg(cmd, cpu_SR[SAR])

            case 8: /
                {
                    TCGv_i64 v = tcg_temp_new_i64();
                    tcg_gen_concat_i32_i64(v, cpu_R[RRR_T], cpu_R[RRR_S]);
                    gen_shift(shr);
                }
                break;

            case 9: /
                if (dc->sar_5bit) {
                    tcg_gen_shr_i32(cpu_R[RRR_R], cpu_R[RRR_T], cpu_SR[SAR]);
                } else {
                    TCGv_i64 v = tcg_temp_new_i64();
                    tcg_gen_extu_i32_i64(v, cpu_R[RRR_T]);
                    gen_shift(shr);
                }
                break;

            case 10: /
                if (dc->sar_m32_5bit) {
                    tcg_gen_shl_i32(cpu_R[RRR_R], cpu_R[RRR_S], dc->sar_m32);
                } else {
                    TCGv_i64 v = tcg_temp_new_i64();
                    TCGv_i32 s = tcg_const_i32(32);
                    tcg_gen_sub_i32(s, s, cpu_SR[SAR]);
                    tcg_gen_andi_i32(s, s, 0x3f);
                    tcg_gen_extu_i32_i64(v, cpu_R[RRR_S]);
                    gen_shift_reg(shl, s);
                    tcg_temp_free(s);
                }
                break;

            case 11: /
                if (dc->sar_5bit) {
                    tcg_gen_sar_i32(cpu_R[RRR_R], cpu_R[RRR_T], cpu_SR[SAR]);
                } else {
                    TCGv_i64 v = tcg_temp_new_i64();
                    tcg_gen_ext_i32_i64(v, cpu_R[RRR_T]);
                    gen_shift(sar);
                }
                break;
#undef gen_shift
#undef gen_shift_reg

            case 12: /
                HAS_OPTION(XTENSA_OPTION_16_BIT_IMUL);
                {
                    TCGv_i32 v1 = tcg_temp_new_i32();
                    TCGv_i32 v2 = tcg_temp_new_i32();
                    tcg_gen_ext16u_i32(v1, cpu_R[RRR_S]);
                    tcg_gen_ext16u_i32(v2, cpu_R[RRR_T]);
                    tcg_gen_mul_i32(cpu_R[RRR_R], v1, v2);
                    tcg_temp_free(v2);
                    tcg_temp_free(v1);
                }
                break;

            case 13: /
                HAS_OPTION(XTENSA_OPTION_16_BIT_IMUL);
                {
                    TCGv_i32 v1 = tcg_temp_new_i32();
                    TCGv_i32 v2 = tcg_temp_new_i32();
                    tcg_gen_ext16s_i32(v1, cpu_R[RRR_S]);
                    tcg_gen_ext16s_i32(v2, cpu_R[RRR_T]);
                    tcg_gen_mul_i32(cpu_R[RRR_R], v1, v2);
                    tcg_temp_free(v2);
                    tcg_temp_free(v1);
                }
                break;

            default: /
                RESERVED();
                break;
            }
            break;

        case 2: /
            if (OP2 >= 12) {
                HAS_OPTION(XTENSA_OPTION_32_BIT_IDIV);
                int label = gen_new_label();
                tcg_gen_brcondi_i32(TCG_COND_NE, cpu_R[RRR_T], 0, label);
                gen_exception_cause(dc, INTEGER_DIVIDE_BY_ZERO_CAUSE);
                gen_set_label(label);
            }

            switch (OP2) {
            case 8: /
                HAS_OPTION(XTENSA_OPTION_32_BIT_IMUL);
                tcg_gen_mul_i32(cpu_R[RRR_R], cpu_R[RRR_S], cpu_R[RRR_T]);
                break;

            case 10: /
            case 11: /
                HAS_OPTION(XTENSA_OPTION_32_BIT_IMUL);
                {
                    TCGv_i64 r = tcg_temp_new_i64();
                    TCGv_i64 s = tcg_temp_new_i64();
                    TCGv_i64 t = tcg_temp_new_i64();

                    if (OP2 == 10) {
                        tcg_gen_extu_i32_i64(s, cpu_R[RRR_S]);
                        tcg_gen_extu_i32_i64(t, cpu_R[RRR_T]);
                    } else {
                        tcg_gen_ext_i32_i64(s, cpu_R[RRR_S]);
                        tcg_gen_ext_i32_i64(t, cpu_R[RRR_T]);
                    }
                    tcg_gen_mul_i64(r, s, t);
                    tcg_gen_shri_i64(r, r, 32);
                    tcg_gen_trunc_i64_i32(cpu_R[RRR_R], r);

                    tcg_temp_free_i64(r);
                    tcg_temp_free_i64(s);
                    tcg_temp_free_i64(t);
                }
                break;

            case 12: /
                tcg_gen_divu_i32(cpu_R[RRR_R], cpu_R[RRR_S], cpu_R[RRR_T]);
                break;

            case 13: /
            case 15: /
                {
                    int label1 = gen_new_label();
                    int label2 = gen_new_label();

                    tcg_gen_brcondi_i32(TCG_COND_NE, cpu_R[RRR_S], 0x80000000,
                            label1);
                    tcg_gen_brcondi_i32(TCG_COND_NE, cpu_R[RRR_T], 0xffffffff,
                            label1);
                    tcg_gen_movi_i32(cpu_R[RRR_R],
                            OP2 == 13 ? 0x80000000 : 0);
                    tcg_gen_br(label2);
                    gen_set_label(label1);
                    if (OP2 == 13) {
                        tcg_gen_div_i32(cpu_R[RRR_R],
                                cpu_R[RRR_S], cpu_R[RRR_T]);
                    } else {
                        tcg_gen_rem_i32(cpu_R[RRR_R],
                                cpu_R[RRR_S], cpu_R[RRR_T]);
                    }
                    gen_set_label(label2);
                }
                break;

            case 14: /
                tcg_gen_remu_i32(cpu_R[RRR_R], cpu_R[RRR_S], cpu_R[RRR_T]);
                break;

            default: /
                RESERVED();
                break;
            }
            break;

        case 3: /
            switch (OP2) {
            case 0: /
                if (RSR_SR >= 64) {
                    gen_check_privilege(dc);
                }
                gen_rsr(dc, cpu_R[RRR_T], RSR_SR);
                if (!sregnames[RSR_SR]) {
                    TBD();
                }
                break;

            case 1: /
                if (RSR_SR >= 64) {
                    gen_check_privilege(dc);
                }
                gen_wsr(dc, RSR_SR, cpu_R[RRR_T]);
                if (!sregnames[RSR_SR]) {
                    TBD();
                }
                break;

            case 2: /
                HAS_OPTION(XTENSA_OPTION_MISC_OP);
                {
                    int shift = 24 - RRR_T;

                    if (shift == 24) {
                        tcg_gen_ext8s_i32(cpu_R[RRR_R], cpu_R[RRR_S]);
                    } else if (shift == 16) {
                        tcg_gen_ext16s_i32(cpu_R[RRR_R], cpu_R[RRR_S]);
                    } else {
                        TCGv_i32 tmp = tcg_temp_new_i32();
                        tcg_gen_shli_i32(tmp, cpu_R[RRR_S], shift);
                        tcg_gen_sari_i32(cpu_R[RRR_R], tmp, shift);
                        tcg_temp_free(tmp);
                    }
                }
                break;

            case 3: /
                HAS_OPTION(XTENSA_OPTION_MISC_OP);
                {
                    TCGv_i32 tmp1 = tcg_temp_new_i32();
                    TCGv_i32 tmp2 = tcg_temp_new_i32();
                    int label = gen_new_label();

                    tcg_gen_sari_i32(tmp1, cpu_R[RRR_S], 24 - RRR_T);
                    tcg_gen_xor_i32(tmp2, tmp1, cpu_R[RRR_S]);
                    tcg_gen_andi_i32(tmp2, tmp2, 0xffffffff << (RRR_T + 7));
                    tcg_gen_mov_i32(cpu_R[RRR_R], cpu_R[RRR_S]);
                    tcg_gen_brcondi_i32(TCG_COND_EQ, tmp2, 0, label);

                    tcg_gen_sari_i32(tmp1, cpu_R[RRR_S], 31);
                    tcg_gen_xori_i32(cpu_R[RRR_R], tmp1,
                            0xffffffff >> (25 - RRR_T));

                    gen_set_label(label);

                    tcg_temp_free(tmp1);
                    tcg_temp_free(tmp2);
                }
                break;

            case 4: /
            case 5: /
            case 6: /
            case 7: /
                HAS_OPTION(XTENSA_OPTION_MISC_OP);
                {
                    static const TCGCond cond[] = {
                        TCG_COND_LE,
                        TCG_COND_GE,
                        TCG_COND_LEU,
                        TCG_COND_GEU
                    };
                    int label = gen_new_label();

                    if (RRR_R != RRR_T) {
                        tcg_gen_mov_i32(cpu_R[RRR_R], cpu_R[RRR_S]);
                        tcg_gen_brcond_i32(cond[OP2 - 4],
                                cpu_R[RRR_S], cpu_R[RRR_T], label);
                        tcg_gen_mov_i32(cpu_R[RRR_R], cpu_R[RRR_T]);
                    } else {
                        tcg_gen_brcond_i32(cond[OP2 - 4],
                                cpu_R[RRR_T], cpu_R[RRR_S], label);
                        tcg_gen_mov_i32(cpu_R[RRR_R], cpu_R[RRR_S]);
                    }
                    gen_set_label(label);
                }
                break;

            case 8: /
            case 9: /
            case 10: /
            case 11: /
                {
                    static const TCGCond cond[] = {
                        TCG_COND_NE,
                        TCG_COND_EQ,
                        TCG_COND_GE,
                        TCG_COND_LT
                    };
                    int label = gen_new_label();
                    tcg_gen_brcondi_i32(cond[OP2 - 8], cpu_R[RRR_T], 0, label);
                    tcg_gen_mov_i32(cpu_R[RRR_R], cpu_R[RRR_S]);
                    gen_set_label(label);
                }
                break;

            case 12: /
                HAS_OPTION(XTENSA_OPTION_BOOLEAN);
                TBD();
                break;

            case 13: /
                HAS_OPTION(XTENSA_OPTION_BOOLEAN);
                TBD();
                break;

            case 14: /
                {
                    int st = (RRR_S << 4) + RRR_T;
                    if (uregnames[st]) {
                        tcg_gen_mov_i32(cpu_R[RRR_R], cpu_UR[st]);
                    } else {
                        qemu_log("RUR %d not implemented, ", st);
                        TBD();
                    }
                }
                break;

            case 15: /
                {
                    if (uregnames[RSR_SR]) {
                        tcg_gen_mov_i32(cpu_UR[RSR_SR], cpu_R[RRR_T]);
                    } else {
                        qemu_log("WUR %d not implemented, ", RSR_SR);
                        TBD();
                    }
                }
                break;

            }
            break;

        case 4: /
        case 5:
            {
                int shiftimm = RRR_S | (OP1 << 4);
                int maskimm = (1 << (OP2 + 1)) - 1;

                TCGv_i32 tmp = tcg_temp_new_i32();
                tcg_gen_shri_i32(tmp, cpu_R[RRR_T], shiftimm);
                tcg_gen_andi_i32(cpu_R[RRR_R], tmp, maskimm);
                tcg_temp_free(tmp);
            }
            break;

        case 6: /
            RESERVED();
            break;

        case 7: /
            RESERVED();
            break;

        case 8: /
            HAS_OPTION(XTENSA_OPTION_COPROCESSOR);
            TBD();
            break;

        case 9: /
            TBD();
            break;

        case 10: /
            HAS_OPTION(XTENSA_OPTION_FP_COPROCESSOR);
            TBD();
            break;

        case 11: /
            HAS_OPTION(XTENSA_OPTION_FP_COPROCESSOR);
            TBD();
            break;

        default: /
            RESERVED();
            break;
        }
        break;

    case 1: /
        {
            TCGv_i32 tmp = tcg_const_i32(
                    (0xfffc0000 | (RI16_IMM16 << 2)) +
                    ((dc->pc + 3) & ~3));

            /

            tcg_gen_qemu_ld32u(cpu_R[RRR_T], tmp, dc->cring);
            tcg_temp_free(tmp);
        }
        break;

    case 2: /
#define gen_load_store(type, shift) do { \
            TCGv_i32 addr = tcg_temp_new_i32(); \
            tcg_gen_addi_i32(addr, cpu_R[RRI8_S], RRI8_IMM8 << shift); \
            tcg_gen_qemu_##type(cpu_R[RRI8_T], addr, dc->cring); \
            tcg_temp_free(addr); \
        } while (0)

        switch (RRI8_R) {
        case 0: /
            gen_load_store(ld8u, 0);
            break;

        case 1: /
            gen_load_store(ld16u, 1);
            break;

        case 2: /
            gen_load_store(ld32u, 2);
            break;

        case 4: /
            gen_load_store(st8, 0);
            break;

        case 5: /
            gen_load_store(st16, 1);
            break;

        case 6: /
            gen_load_store(st32, 2);
            break;

        case 7: /
            if (RRI8_T < 8) {
                HAS_OPTION(XTENSA_OPTION_DCACHE);
            }

            switch (RRI8_T) {
            case 0: /
                break;

            case 1: /
                break;

            case 2: /
                break;

            case 3: /
                break;

            case 4: /
                break;

            case 5: /
                break;

            case 6: /
                break;

            case 7: /
                break;

            case 8: /
                switch (OP1) {
                case 0: /
                    HAS_OPTION(XTENSA_OPTION_DCACHE_INDEX_LOCK);
                    break;

                case 2: /
                    HAS_OPTION(XTENSA_OPTION_DCACHE_INDEX_LOCK);
                    break;

                case 3: /
                    HAS_OPTION(XTENSA_OPTION_DCACHE_INDEX_LOCK);
                    break;

                case 4: /
                    HAS_OPTION(XTENSA_OPTION_DCACHE);
                    break;

                case 5: /
                    HAS_OPTION(XTENSA_OPTION_DCACHE);
                    break;

                default: /
                    RESERVED();
                    break;

                }
                break;

            case 12: /
                HAS_OPTION(XTENSA_OPTION_ICACHE);
                break;

            case 13: /
                switch (OP1) {
                case 0: /
                    HAS_OPTION(XTENSA_OPTION_ICACHE_INDEX_LOCK);
                    break;

                case 2: /
                    HAS_OPTION(XTENSA_OPTION_ICACHE_INDEX_LOCK);
                    break;

                case 3: /
                    HAS_OPTION(XTENSA_OPTION_ICACHE_INDEX_LOCK);
                    break;

                default: /
                    RESERVED();
                    break;
                }
                break;

            case 14: /
                HAS_OPTION(XTENSA_OPTION_ICACHE);
                break;

            case 15: /
                HAS_OPTION(XTENSA_OPTION_ICACHE);
                break;

            default: /
                RESERVED();
                break;
            }
            break;

        case 9: /
            gen_load_store(ld16s, 1);
            break;

        case 10: /
            tcg_gen_movi_i32(cpu_R[RRI8_T],
                    RRI8_IMM8 | (RRI8_S << 8) |
                    ((RRI8_S & 0x8) ? 0xfffff000 : 0));
            break;

        case 11: /
            HAS_OPTION(XTENSA_OPTION_MP_SYNCHRO);
            gen_load_store(ld32u, 2); /
            break;

        case 12: /
            tcg_gen_addi_i32(cpu_R[RRI8_T], cpu_R[RRI8_S], RRI8_IMM8_SE);
            break;

        case 13: /
            tcg_gen_addi_i32(cpu_R[RRI8_T], cpu_R[RRI8_S], RRI8_IMM8_SE << 8);
            break;

        case 14: /
            HAS_OPTION(XTENSA_OPTION_MP_SYNCHRO);
            {
                int label = gen_new_label();
                TCGv_i32 tmp = tcg_temp_local_new_i32();
                TCGv_i32 addr = tcg_temp_local_new_i32();

                tcg_gen_mov_i32(tmp, cpu_R[RRI8_T]);
                tcg_gen_addi_i32(addr, cpu_R[RRI8_S], RRI8_IMM8 << 2);
                tcg_gen_qemu_ld32u(cpu_R[RRI8_T], addr, dc->cring);
                tcg_gen_brcond_i32(TCG_COND_NE, cpu_R[RRI8_T],
                        cpu_SR[SCOMPARE1], label);

                tcg_gen_qemu_st32(tmp, addr, dc->cring);

                gen_set_label(label);
                tcg_temp_free(addr);
                tcg_temp_free(tmp);
            }
            break;

        case 15: /
            HAS_OPTION(XTENSA_OPTION_MP_SYNCHRO);
            gen_load_store(st32, 2); /
            break;

        default: /
            RESERVED();
            break;
        }
        break;
#undef gen_load_store

    case 3: /
        HAS_OPTION(XTENSA_OPTION_COPROCESSOR);
        TBD();
        break;

    case 4: /
        HAS_OPTION(XTENSA_OPTION_MAC16);
        TBD();
        break;

    case 5: /
        switch (CALL_N) {
        case 0: /
            tcg_gen_movi_i32(cpu_R[0], dc->next_pc);
            gen_jumpi(dc, (dc->pc & ~3) + (CALL_OFFSET_SE << 2) + 4, 0);
            break;

        case 1: /
        case 2: /
        case 3: /
            HAS_OPTION(XTENSA_OPTION_WINDOWED_REGISTER);
            TBD();
            break;
        }
        break;

    case 6: /
        switch (CALL_N) {
        case 0: /
            gen_jumpi(dc, dc->pc + 4 + CALL_OFFSET_SE, 0);
            break;

        case 1: /
            {
                static const TCGCond cond[] = {
                    TCG_COND_EQ, /
                    TCG_COND_NE, /
                    TCG_COND_LT, /
                    TCG_COND_GE, /
                };

                gen_brcondi(dc, cond[BRI12_M & 3], cpu_R[BRI12_S], 0,
                        4 + BRI12_IMM12_SE);
            }
            break;

        case 2: /
            {
                static const TCGCond cond[] = {
                    TCG_COND_EQ, /
                    TCG_COND_NE, /
                    TCG_COND_LT, /
                    TCG_COND_GE, /
                };

                gen_brcondi(dc, cond[BRI8_M & 3],
                        cpu_R[BRI8_S], B4CONST[BRI8_R], 4 + BRI8_IMM8_SE);
            }
            break;

        case 3: /
            switch (BRI8_M) {
            case 0: /
                HAS_OPTION(XTENSA_OPTION_WINDOWED_REGISTER);
                TBD();
                break;

            case 1: /
                switch (BRI8_R) {
                case 0: /
                    HAS_OPTION(XTENSA_OPTION_BOOLEAN);
                    TBD();
                    break;

                case 1: /
                    HAS_OPTION(XTENSA_OPTION_BOOLEAN);
                    TBD();
                    break;

                case 8: /
                    TBD();
                    break;

                case 9: /
                    TBD();
                    break;

                case 10: /
                    TBD();
                    break;

                default: /
                    RESERVED();
                    break;

                }
                break;

            case 2: /
            case 3: /
                gen_brcondi(dc, BRI8_M == 2 ? TCG_COND_LTU : TCG_COND_GEU,
                        cpu_R[BRI8_S], B4CONSTU[BRI8_R], 4 + BRI8_IMM8_SE);
                break;
            }
            break;

        }
        break;

    case 7: /
        {
            TCGCond eq_ne = (RRI8_R & 8) ? TCG_COND_NE : TCG_COND_EQ;

            switch (RRI8_R & 7) {
            case 0: / /
                {
                    TCGv_i32 tmp = tcg_temp_new_i32();
                    tcg_gen_and_i32(tmp, cpu_R[RRI8_S], cpu_R[RRI8_T]);
                    gen_brcondi(dc, eq_ne, tmp, 0, 4 + RRI8_IMM8_SE);
                    tcg_temp_free(tmp);
                }
                break;

            case 1: / /
            case 2: / /
            case 3: / /
                {
                    static const TCGCond cond[] = {
                        [1] = TCG_COND_EQ,
                        [2] = TCG_COND_LT,
                        [3] = TCG_COND_LTU,
                        [9] = TCG_COND_NE,
                        [10] = TCG_COND_GE,
                        [11] = TCG_COND_GEU,
                    };
                    gen_brcond(dc, cond[RRI8_R], cpu_R[RRI8_S], cpu_R[RRI8_T],
                            4 + RRI8_IMM8_SE);
                }
                break;

            case 4: / /
                {
                    TCGv_i32 tmp = tcg_temp_new_i32();
                    tcg_gen_and_i32(tmp, cpu_R[RRI8_S], cpu_R[RRI8_T]);
                    gen_brcond(dc, eq_ne, tmp, cpu_R[RRI8_T],
                            4 + RRI8_IMM8_SE);
                    tcg_temp_free(tmp);
                }
                break;

            case 5: / /
                {
                    TCGv_i32 bit = tcg_const_i32(1);
                    TCGv_i32 tmp = tcg_temp_new_i32();
                    tcg_gen_andi_i32(tmp, cpu_R[RRI8_T], 0x1f);
                    tcg_gen_shl_i32(bit, bit, tmp);
                    tcg_gen_and_i32(tmp, cpu_R[RRI8_S], bit);
                    gen_brcondi(dc, eq_ne, tmp, 0, 4 + RRI8_IMM8_SE);
                    tcg_temp_free(tmp);
                    tcg_temp_free(bit);
                }
                break;

            case 6: / /
            case 7:
                {
                    TCGv_i32 tmp = tcg_temp_new_i32();
                    tcg_gen_andi_i32(tmp, cpu_R[RRI8_S],
                            1 << (((RRI8_R & 1) << 4) | RRI8_T));
                    gen_brcondi(dc, eq_ne, tmp, 0, 4 + RRI8_IMM8_SE);
                    tcg_temp_free(tmp);
                }
                break;

            }
        }
        break;

#define gen_narrow_load_store(type) do { \
            TCGv_i32 addr = tcg_temp_new_i32(); \
            tcg_gen_addi_i32(addr, cpu_R[RRRN_S], RRRN_R << 2); \
            tcg_gen_qemu_##type(cpu_R[RRRN_T], addr, dc->cring); \
            tcg_temp_free(addr); \
        } while (0)

    case 8: /
        gen_narrow_load_store(ld32u);
        break;

    case 9: /
        gen_narrow_load_store(st32);
        break;
#undef gen_narrow_load_store

    case 10: /
        tcg_gen_add_i32(cpu_R[RRRN_R], cpu_R[RRRN_S], cpu_R[RRRN_T]);
        break;

    case 11: /
        tcg_gen_addi_i32(cpu_R[RRRN_R], cpu_R[RRRN_S], RRRN_T ? RRRN_T : -1);
        break;

    case 12: /
        if (RRRN_T < 8) { /
            tcg_gen_movi_i32(cpu_R[RRRN_S],
                    RRRN_R | (RRRN_T << 4) |
                    ((RRRN_T & 6) == 6 ? 0xffffff80 : 0));
        } else { / /
            TCGCond eq_ne = (RRRN_T & 4) ? TCG_COND_NE : TCG_COND_EQ;

            gen_brcondi(dc, eq_ne, cpu_R[RRRN_S], 0,
                    4 + (RRRN_R | ((RRRN_T & 3) << 4)));
        }
        break;

    case 13: /
        switch (RRRN_R) {
        case 0: /
            tcg_gen_mov_i32(cpu_R[RRRN_T], cpu_R[RRRN_S]);
            break;

        case 15: /
            switch (RRRN_T) {
            case 0: /
                gen_jump(dc, cpu_R[0]);
                break;

            case 1: /
                HAS_OPTION(XTENSA_OPTION_WINDOWED_REGISTER);
                TBD();
                break;

            case 2: /
                TBD();
                break;

            case 3: /
                break;

            case 6: /
                gen_exception_cause(dc, ILLEGAL_INSTRUCTION_CAUSE);
                break;

            default: /
                RESERVED();
                break;
            }
            break;

        default: /
            RESERVED();
            break;
        }
        break;

    default: /
        RESERVED();
        break;
    }

    dc->pc = dc->next_pc;
    return;

invalid_opcode:
    qemu_log("INVALID(pc = %08x)\n", dc->pc);
    dc->pc = dc->next_pc;
#undef HAS_OPTION
}
