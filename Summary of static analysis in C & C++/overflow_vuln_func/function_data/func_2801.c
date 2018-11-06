static void disas_arm_insn(CPUState * env, DisasContext *s)
{
    unsigned int cond, insn, val, op1, i, shift, rm, rs, rn, rd, sh;
    TCGv tmp;
    TCGv tmp2;
    TCGv tmp3;
    TCGv addr;
    TCGv_i64 tmp64;

    insn = ldl_code(s->pc);
    s->pc += 4;

    /
    if (IS_M(env))
        goto illegal_op;
    cond = insn >> 28;
    if (cond == 0xf){
        /
        if (((insn >> 25) & 7) == 1) {
            /
            if (!arm_feature(env, ARM_FEATURE_NEON))
                goto illegal_op;

            if (disas_neon_data_insn(env, s, insn))
                goto illegal_op;
            return;
        }
        if ((insn & 0x0f100000) == 0x04000000) {
            /
            if (!arm_feature(env, ARM_FEATURE_NEON))
                goto illegal_op;

            if (disas_neon_ls_insn(env, s, insn))
                goto illegal_op;
            return;
        }
        if (((insn & 0x0f30f000) == 0x0510f000) ||
            ((insn & 0x0f30f010) == 0x0710f000)) {
            if ((insn & (1 << 22)) == 0) {
                /
                if (!arm_feature(env, ARM_FEATURE_V7MP)) {
                    goto illegal_op;
                }
            }
            /
            return;
        }
        if (((insn & 0x0f70f000) == 0x0450f000) ||
            ((insn & 0x0f70f010) == 0x0650f000)) {
            ARCH(7);
            return; /
        }
        if (((insn & 0x0f700000) == 0x04100000) ||
            ((insn & 0x0f700010) == 0x06100000)) {
            if (!arm_feature(env, ARM_FEATURE_V7MP)) {
                goto illegal_op;
            }
            return; /
        }

        if ((insn & 0x0ffffdff) == 0x01010000) {
            ARCH(6);
            /
            if (insn & (1 << 9)) {
                /
                goto illegal_op;
            }
            return;
        } else if ((insn & 0x0fffff00) == 0x057ff000) {
            switch ((insn >> 4) & 0xf) {
            case 1: /
                ARCH(6K);
                gen_clrex(s);
                return;
            case 4: /
            case 5: /
            case 6: /
                ARCH(7);
                /
                return;
            default:
                goto illegal_op;
            }
        } else if ((insn & 0x0e5fffe0) == 0x084d0500) {
            /
            int32_t offset;
            if (IS_USER(s))
                goto illegal_op;
            ARCH(6);
            op1 = (insn & 0x1f);
            addr = tcg_temp_new_i32();
            tmp = tcg_const_i32(op1);
            gen_helper_get_r13_banked(addr, cpu_env, tmp);
            tcg_temp_free_i32(tmp);
            i = (insn >> 23) & 3;
            switch (i) {
            case 0: offset = -4; break; /
            case 1: offset = 0; break; /
            case 2: offset = -8; break; /
            case 3: offset = 4; break; /
            default: abort();
            }
            if (offset)
                tcg_gen_addi_i32(addr, addr, offset);
            tmp = load_reg(s, 14);
            gen_st32(tmp, addr, 0);
            tmp = load_cpu_field(spsr);
            tcg_gen_addi_i32(addr, addr, 4);
            gen_st32(tmp, addr, 0);
            if (insn & (1 << 21)) {
                /
                switch (i) {
                case 0: offset = -8; break;
                case 1: offset = 4; break;
                case 2: offset = -4; break;
                case 3: offset = 0; break;
                default: abort();
                }
                if (offset)
                    tcg_gen_addi_i32(addr, addr, offset);
                tmp = tcg_const_i32(op1);
                gen_helper_set_r13_banked(cpu_env, tmp, addr);
                tcg_temp_free_i32(tmp);
                tcg_temp_free_i32(addr);
            } else {
                tcg_temp_free_i32(addr);
            }
            return;
        } else if ((insn & 0x0e50ffe0) == 0x08100a00) {
            /
            int32_t offset;
            if (IS_USER(s))
                goto illegal_op;
            ARCH(6);
            rn = (insn >> 16) & 0xf;
            addr = load_reg(s, rn);
            i = (insn >> 23) & 3;
            switch (i) {
            case 0: offset = -4; break; /
            case 1: offset = 0; break; /
            case 2: offset = -8; break; /
            case 3: offset = 4; break; /
            default: abort();
            }
            if (offset)
                tcg_gen_addi_i32(addr, addr, offset);
            /
            tmp = gen_ld32(addr, 0);
            tcg_gen_addi_i32(addr, addr, 4);
            tmp2 = gen_ld32(addr, 0);
            if (insn & (1 << 21)) {
                /
                switch (i) {
                case 0: offset = -8; break;
                case 1: offset = 4; break;
                case 2: offset = -4; break;
                case 3: offset = 0; break;
                default: abort();
                }
                if (offset)
                    tcg_gen_addi_i32(addr, addr, offset);
                store_reg(s, rn, addr);
            } else {
                tcg_temp_free_i32(addr);
            }
            gen_rfe(s, tmp, tmp2);
            return;
        } else if ((insn & 0x0e000000) == 0x0a000000) {
            /
            int32_t offset;

            val = (uint32_t)s->pc;
            tmp = tcg_temp_new_i32();
            tcg_gen_movi_i32(tmp, val);
            store_reg(s, 14, tmp);
            /
            offset = (((int32_t)insn) << 8) >> 8;
            /
            val += (offset << 2) | ((insn >> 23) & 2) | 1;
            /
            val += 4;
            gen_bx_im(s, val);
            return;
        } else if ((insn & 0x0e000f00) == 0x0c000100) {
            if (arm_feature(env, ARM_FEATURE_IWMMXT)) {
                /
                if (env->cp15.c15_cpar & (1 << 1))
                    if (!disas_iwmmxt_insn(env, s, insn))
                        return;
            }
        } else if ((insn & 0x0fe00000) == 0x0c400000) {
            /
        } else if ((insn & 0x0f000010) == 0x0e000010) {
            /
        } else if ((insn & 0x0ff10020) == 0x01000000) {
            uint32_t mask;
            uint32_t val;
            /
            if (IS_USER(s))
                return;
            mask = val = 0;
            if (insn & (1 << 19)) {
                if (insn & (1 << 8))
                    mask |= CPSR_A;
                if (insn & (1 << 7))
                    mask |= CPSR_I;
                if (insn & (1 << 6))
                    mask |= CPSR_F;
                if (insn & (1 << 18))
                    val |= mask;
            }
            if (insn & (1 << 17)) {
                mask |= CPSR_M;
                val |= (insn & 0x1f);
            }
            if (mask) {
                gen_set_psr_im(s, mask, 0, val);
            }
            return;
        }
        goto illegal_op;
    }
    if (cond != 0xe) {
        /
        s->condlabel = gen_new_label();
        gen_test_cc(cond ^ 1, s->condlabel);
        s->condjmp = 1;
    }
    if ((insn & 0x0f900000) == 0x03000000) {
        if ((insn & (1 << 21)) == 0) {
            ARCH(6T2);
            rd = (insn >> 12) & 0xf;
            val = ((insn >> 4) & 0xf000) | (insn & 0xfff);
            if ((insn & (1 << 22)) == 0) {
                /
                tmp = tcg_temp_new_i32();
                tcg_gen_movi_i32(tmp, val);
            } else {
                /
                tmp = load_reg(s, rd);
                tcg_gen_ext16u_i32(tmp, tmp);
                tcg_gen_ori_i32(tmp, tmp, val << 16);
            }
            store_reg(s, rd, tmp);
        } else {
            if (((insn >> 12) & 0xf) != 0xf)
                goto illegal_op;
            if (((insn >> 16) & 0xf) == 0) {
                gen_nop_hint(s, insn & 0xff);
            } else {
                /
                val = insn & 0xff;
                shift = ((insn >> 8) & 0xf) * 2;
                if (shift)
                    val = (val >> shift) | (val << (32 - shift));
                i = ((insn & (1 << 22)) != 0);
                if (gen_set_psr_im(s, msr_mask(env, s, (insn >> 16) & 0xf, i), i, val))
                    goto illegal_op;
            }
        }
    } else if ((insn & 0x0f900000) == 0x01000000
               && (insn & 0x00000090) != 0x00000090) {
        /
        op1 = (insn >> 21) & 3;
        sh = (insn >> 4) & 0xf;
        rm = insn & 0xf;
        switch (sh) {
        case 0x0: /
            if (op1 & 1) {
                /
                tmp = load_reg(s, rm);
                i = ((op1 & 2) != 0);
                if (gen_set_psr(s, msr_mask(env, s, (insn >> 16) & 0xf, i), i, tmp))
                    goto illegal_op;
            } else {
                /
                rd = (insn >> 12) & 0xf;
                if (op1 & 2) {
                    if (IS_USER(s))
                        goto illegal_op;
                    tmp = load_cpu_field(spsr);
                } else {
                    tmp = tcg_temp_new_i32();
                    gen_helper_cpsr_read(tmp);
                }
                store_reg(s, rd, tmp);
            }
            break;
        case 0x1:
            if (op1 == 1) {
                /
                tmp = load_reg(s, rm);
                gen_bx(s, tmp);
            } else if (op1 == 3) {
                /
                rd = (insn >> 12) & 0xf;
                tmp = load_reg(s, rm);
                gen_helper_clz(tmp, tmp);
                store_reg(s, rd, tmp);
            } else {
                goto illegal_op;
            }
            break;
        case 0x2:
            if (op1 == 1) {
                ARCH(5J); /
                /
                tmp = load_reg(s, rm);
                gen_bx(s, tmp);
            } else {
                goto illegal_op;
            }
            break;
        case 0x3:
            if (op1 != 1)
              goto illegal_op;

            /
            tmp = load_reg(s, rm);
            tmp2 = tcg_temp_new_i32();
            tcg_gen_movi_i32(tmp2, s->pc);
            store_reg(s, 14, tmp2);
            gen_bx(s, tmp);
            break;
        case 0x5: /
            rd = (insn >> 12) & 0xf;
            rn = (insn >> 16) & 0xf;
            tmp = load_reg(s, rm);
            tmp2 = load_reg(s, rn);
            if (op1 & 2)
                gen_helper_double_saturate(tmp2, tmp2);
            if (op1 & 1)
                gen_helper_sub_saturate(tmp, tmp, tmp2);
            else
                gen_helper_add_saturate(tmp, tmp, tmp2);
            tcg_temp_free_i32(tmp2);
            store_reg(s, rd, tmp);
            break;
        case 7:
            /
            if (op1 != 1) {
                goto illegal_op;
            }
            /
            gen_exception_insn(s, 4, EXCP_BKPT);
            break;
        case 0x8: /
        case 0xa:
        case 0xc:
        case 0xe:
            rs = (insn >> 8) & 0xf;
            rn = (insn >> 12) & 0xf;
            rd = (insn >> 16) & 0xf;
            if (op1 == 1) {
                /
                tmp = load_reg(s, rm);
                tmp2 = load_reg(s, rs);
                if (sh & 4)
                    tcg_gen_sari_i32(tmp2, tmp2, 16);
                else
                    gen_sxth(tmp2);
                tmp64 = gen_muls_i64_i32(tmp, tmp2);
                tcg_gen_shri_i64(tmp64, tmp64, 16);
                tmp = tcg_temp_new_i32();
                tcg_gen_trunc_i64_i32(tmp, tmp64);
                tcg_temp_free_i64(tmp64);
                if ((sh & 2) == 0) {
                    tmp2 = load_reg(s, rn);
                    gen_helper_add_setq(tmp, tmp, tmp2);
                    tcg_temp_free_i32(tmp2);
                }
                store_reg(s, rd, tmp);
            } else {
                /
                tmp = load_reg(s, rm);
                tmp2 = load_reg(s, rs);
                gen_mulxy(tmp, tmp2, sh & 2, sh & 4);
                tcg_temp_free_i32(tmp2);
                if (op1 == 2) {
                    tmp64 = tcg_temp_new_i64();
                    tcg_gen_ext_i32_i64(tmp64, tmp);
                    tcg_temp_free_i32(tmp);
                    gen_addq(s, tmp64, rn, rd);
                    gen_storeq_reg(s, rn, rd, tmp64);
                    tcg_temp_free_i64(tmp64);
                } else {
                    if (op1 == 0) {
                        tmp2 = load_reg(s, rn);
                        gen_helper_add_setq(tmp, tmp, tmp2);
                        tcg_temp_free_i32(tmp2);
                    }
                    store_reg(s, rd, tmp);
                }
            }
            break;
        default:
            goto illegal_op;
        }
    } else if (((insn & 0x0e000000) == 0 &&
                (insn & 0x00000090) != 0x90) ||
               ((insn & 0x0e000000) == (1 << 25))) {
        int set_cc, logic_cc, shiftop;

        op1 = (insn >> 21) & 0xf;
        set_cc = (insn >> 20) & 1;
        logic_cc = table_logic_cc[op1] & set_cc;

        /
        if (insn & (1 << 25)) {
            /
            val = insn & 0xff;
            shift = ((insn >> 8) & 0xf) * 2;
            if (shift) {
                val = (val >> shift) | (val << (32 - shift));
            }
            tmp2 = tcg_temp_new_i32();
            tcg_gen_movi_i32(tmp2, val);
            if (logic_cc && shift) {
                gen_set_CF_bit31(tmp2);
            }
        } else {
            /
            rm = (insn) & 0xf;
            tmp2 = load_reg(s, rm);
            shiftop = (insn >> 5) & 3;
            if (!(insn & (1 << 4))) {
                shift = (insn >> 7) & 0x1f;
                gen_arm_shift_im(tmp2, shiftop, shift, logic_cc);
            } else {
                rs = (insn >> 8) & 0xf;
                tmp = load_reg(s, rs);
                gen_arm_shift_reg(tmp2, shiftop, tmp, logic_cc);
            }
        }
        if (op1 != 0x0f && op1 != 0x0d) {
            rn = (insn >> 16) & 0xf;
            tmp = load_reg(s, rn);
        } else {
            TCGV_UNUSED(tmp);
        }
        rd = (insn >> 12) & 0xf;
        switch(op1) {
        case 0x00:
            tcg_gen_and_i32(tmp, tmp, tmp2);
            if (logic_cc) {
                gen_logic_CC(tmp);
            }
            store_reg_bx(env, s, rd, tmp);
            break;
        case 0x01:
            tcg_gen_xor_i32(tmp, tmp, tmp2);
            if (logic_cc) {
                gen_logic_CC(tmp);
            }
            store_reg_bx(env, s, rd, tmp);
            break;
        case 0x02:
            if (set_cc && rd == 15) {
                /
                if (IS_USER(s)) {
                    goto illegal_op;
                }
                gen_helper_sub_cc(tmp, tmp, tmp2);
                gen_exception_return(s, tmp);
            } else {
                if (set_cc) {
                    gen_helper_sub_cc(tmp, tmp, tmp2);
                } else {
                    tcg_gen_sub_i32(tmp, tmp, tmp2);
                }
                store_reg_bx(env, s, rd, tmp);
            }
            break;
        case 0x03:
            if (set_cc) {
                gen_helper_sub_cc(tmp, tmp2, tmp);
            } else {
                tcg_gen_sub_i32(tmp, tmp2, tmp);
            }
            store_reg_bx(env, s, rd, tmp);
            break;
        case 0x04:
            if (set_cc) {
                gen_helper_add_cc(tmp, tmp, tmp2);
            } else {
                tcg_gen_add_i32(tmp, tmp, tmp2);
            }
            store_reg_bx(env, s, rd, tmp);
            break;
        case 0x05:
            if (set_cc) {
                gen_helper_adc_cc(tmp, tmp, tmp2);
            } else {
                gen_add_carry(tmp, tmp, tmp2);
            }
            store_reg_bx(env, s, rd, tmp);
            break;
        case 0x06:
            if (set_cc) {
                gen_helper_sbc_cc(tmp, tmp, tmp2);
            } else {
                gen_sub_carry(tmp, tmp, tmp2);
            }
            store_reg_bx(env, s, rd, tmp);
            break;
        case 0x07:
            if (set_cc) {
                gen_helper_sbc_cc(tmp, tmp2, tmp);
            } else {
                gen_sub_carry(tmp, tmp2, tmp);
            }
            store_reg_bx(env, s, rd, tmp);
            break;
        case 0x08:
            if (set_cc) {
                tcg_gen_and_i32(tmp, tmp, tmp2);
                gen_logic_CC(tmp);
            }
            tcg_temp_free_i32(tmp);
            break;
        case 0x09:
            if (set_cc) {
                tcg_gen_xor_i32(tmp, tmp, tmp2);
                gen_logic_CC(tmp);
            }
            tcg_temp_free_i32(tmp);
            break;
        case 0x0a:
            if (set_cc) {
                gen_helper_sub_cc(tmp, tmp, tmp2);
            }
            tcg_temp_free_i32(tmp);
            break;
        case 0x0b:
            if (set_cc) {
                gen_helper_add_cc(tmp, tmp, tmp2);
            }
            tcg_temp_free_i32(tmp);
            break;
        case 0x0c:
            tcg_gen_or_i32(tmp, tmp, tmp2);
            if (logic_cc) {
                gen_logic_CC(tmp);
            }
            store_reg_bx(env, s, rd, tmp);
            break;
        case 0x0d:
            if (logic_cc && rd == 15) {
                /
                if (IS_USER(s)) {
                    goto illegal_op;
                }
                gen_exception_return(s, tmp2);
            } else {
                if (logic_cc) {
                    gen_logic_CC(tmp2);
                }
                store_reg_bx(env, s, rd, tmp2);
            }
            break;
        case 0x0e:
            tcg_gen_andc_i32(tmp, tmp, tmp2);
            if (logic_cc) {
                gen_logic_CC(tmp);
            }
            store_reg_bx(env, s, rd, tmp);
            break;
        default:
        case 0x0f:
            tcg_gen_not_i32(tmp2, tmp2);
            if (logic_cc) {
                gen_logic_CC(tmp2);
            }
            store_reg_bx(env, s, rd, tmp2);
            break;
        }
        if (op1 != 0x0f && op1 != 0x0d) {
            tcg_temp_free_i32(tmp2);
        }
    } else {
        /
        op1 = (insn >> 24) & 0xf;
        switch(op1) {
        case 0x0:
        case 0x1:
            /
            sh = (insn >> 5) & 3;
            if (sh == 0) {
                if (op1 == 0x0) {
                    rd = (insn >> 16) & 0xf;
                    rn = (insn >> 12) & 0xf;
                    rs = (insn >> 8) & 0xf;
                    rm = (insn) & 0xf;
                    op1 = (insn >> 20) & 0xf;
                    switch (op1) {
                    case 0: case 1: case 2: case 3: case 6:
                        /
                        tmp = load_reg(s, rs);
                        tmp2 = load_reg(s, rm);
                        tcg_gen_mul_i32(tmp, tmp, tmp2);
                        tcg_temp_free_i32(tmp2);
                        if (insn & (1 << 22)) {
                            /
                            ARCH(6T2);
                            tmp2 = load_reg(s, rn);
                            tcg_gen_sub_i32(tmp, tmp2, tmp);
                            tcg_temp_free_i32(tmp2);
                        } else if (insn & (1 << 21)) {
                            /
                            tmp2 = load_reg(s, rn);
                            tcg_gen_add_i32(tmp, tmp, tmp2);
                            tcg_temp_free_i32(tmp2);
                        }
                        if (insn & (1 << 20))
                            gen_logic_CC(tmp);
                        store_reg(s, rd, tmp);
                        break;
                    case 4:
                        /
                        ARCH(6);
                        tmp = load_reg(s, rs);
                        tmp2 = load_reg(s, rm);
                        tmp64 = gen_mulu_i64_i32(tmp, tmp2);
                        gen_addq_lo(s, tmp64, rn);
                        gen_addq_lo(s, tmp64, rd);
                        gen_storeq_reg(s, rn, rd, tmp64);
                        tcg_temp_free_i64(tmp64);
                        break;
                    case 8: case 9: case 10: case 11:
                    case 12: case 13: case 14: case 15:
                        /
                        tmp = load_reg(s, rs);
                        tmp2 = load_reg(s, rm);
                        if (insn & (1 << 22)) {
                            tmp64 = gen_muls_i64_i32(tmp, tmp2);
                        } else {
                            tmp64 = gen_mulu_i64_i32(tmp, tmp2);
                        }
                        if (insn & (1 << 21)) { /
                            gen_addq(s, tmp64, rn, rd);
                        }
                        if (insn & (1 << 20)) {
                            gen_logicq_cc(tmp64);
                        }
                        gen_storeq_reg(s, rn, rd, tmp64);
                        tcg_temp_free_i64(tmp64);
                        break;
                    default:
                        goto illegal_op;
                    }
                } else {
                    rn = (insn >> 16) & 0xf;
                    rd = (insn >> 12) & 0xf;
                    if (insn & (1 << 23)) {
                        /
                        op1 = (insn >> 21) & 0x3;
                        if (op1)
                            ARCH(6K);
                        else
                            ARCH(6);
                        addr = tcg_temp_local_new_i32();
                        load_reg_var(s, addr, rn);
                        if (insn & (1 << 20)) {
                            switch (op1) {
                            case 0: /
                                gen_load_exclusive(s, rd, 15, addr, 2);
                                break;
                            case 1: /
                                gen_load_exclusive(s, rd, rd + 1, addr, 3);
                                break;
                            case 2: /
                                gen_load_exclusive(s, rd, 15, addr, 0);
                                break;
                            case 3: /
                                gen_load_exclusive(s, rd, 15, addr, 1);
                                break;
                            default:
                                abort();
                            }
                        } else {
                            rm = insn & 0xf;
                            switch (op1) {
                            case 0:  /
                                gen_store_exclusive(s, rd, rm, 15, addr, 2);
                                break;
                            case 1: /
                                gen_store_exclusive(s, rd, rm, rm + 1, addr, 3);
                                break;
                            case 2: /
                                gen_store_exclusive(s, rd, rm, 15, addr, 0);
                                break;
                            case 3: /
                                gen_store_exclusive(s, rd, rm, 15, addr, 1);
                                break;
                            default:
                                abort();
                            }
                        }
                        tcg_temp_free(addr);
                    } else {
                        /
                        rm = (insn) & 0xf;

                        /
                        addr = load_reg(s, rn);
                        tmp = load_reg(s, rm);
                        if (insn & (1 << 22)) {
                            tmp2 = gen_ld8u(addr, IS_USER(s));
                            gen_st8(tmp, addr, IS_USER(s));
                        } else {
                            tmp2 = gen_ld32(addr, IS_USER(s));
                            gen_st32(tmp, addr, IS_USER(s));
                        }
                        tcg_temp_free_i32(addr);
                        store_reg(s, rd, tmp2);
                    }
                }
            } else {
                int address_offset;
                int load;
                /
                rn = (insn >> 16) & 0xf;
                rd = (insn >> 12) & 0xf;
                addr = load_reg(s, rn);
                if (insn & (1 << 24))
                    gen_add_datah_offset(s, insn, 0, addr);
                address_offset = 0;
                if (insn & (1 << 20)) {
                    /
                    switch(sh) {
                    case 1:
                        tmp = gen_ld16u(addr, IS_USER(s));
                        break;
                    case 2:
                        tmp = gen_ld8s(addr, IS_USER(s));
                        break;
                    default:
                    case 3:
                        tmp = gen_ld16s(addr, IS_USER(s));
                        break;
                    }
                    load = 1;
                } else if (sh & 2) {
                    /
                    if (sh & 1) {
                        /
                        tmp = load_reg(s, rd);
                        gen_st32(tmp, addr, IS_USER(s));
                        tcg_gen_addi_i32(addr, addr, 4);
                        tmp = load_reg(s, rd + 1);
                        gen_st32(tmp, addr, IS_USER(s));
                        load = 0;
                    } else {
                        /
                        tmp = gen_ld32(addr, IS_USER(s));
                        store_reg(s, rd, tmp);
                        tcg_gen_addi_i32(addr, addr, 4);
                        tmp = gen_ld32(addr, IS_USER(s));
                        rd++;
                        load = 1;
                    }
                    address_offset = -4;
                } else {
                    /
                    tmp = load_reg(s, rd);
                    gen_st16(tmp, addr, IS_USER(s));
                    load = 0;
                }
                /
                if (!(insn & (1 << 24))) {
                    gen_add_datah_offset(s, insn, address_offset, addr);
                    store_reg(s, rn, addr);
                } else if (insn & (1 << 21)) {
                    if (address_offset)
                        tcg_gen_addi_i32(addr, addr, address_offset);
                    store_reg(s, rn, addr);
                } else {
                    tcg_temp_free_i32(addr);
                }
                if (load) {
                    /
                    store_reg(s, rd, tmp);
                }
            }
            break;
        case 0x4:
        case 0x5:
            goto do_ldst;
        case 0x6:
        case 0x7:
            if (insn & (1 << 4)) {
                ARCH(6);
                /
                rm = insn & 0xf;
                rn = (insn >> 16) & 0xf;
                rd = (insn >> 12) & 0xf;
                rs = (insn >> 8) & 0xf;
                switch ((insn >> 23) & 3) {
                case 0: /
                    op1 = (insn >> 20) & 7;
                    tmp = load_reg(s, rn);
                    tmp2 = load_reg(s, rm);
                    sh = (insn >> 5) & 7;
                    if ((op1 & 3) == 0 || sh == 5 || sh == 6)
                        goto illegal_op;
                    gen_arm_parallel_addsub(op1, sh, tmp, tmp2);
                    tcg_temp_free_i32(tmp2);
                    store_reg(s, rd, tmp);
                    break;
                case 1:
                    if ((insn & 0x00700020) == 0) {
                        /
                        tmp = load_reg(s, rn);
                        tmp2 = load_reg(s, rm);
                        shift = (insn >> 7) & 0x1f;
                        if (insn & (1 << 6)) {
                            /
                            if (shift == 0)
                                shift = 31;
                            tcg_gen_sari_i32(tmp2, tmp2, shift);
                            tcg_gen_andi_i32(tmp, tmp, 0xffff0000);
                            tcg_gen_ext16u_i32(tmp2, tmp2);
                        } else {
                            /
                            if (shift)
                                tcg_gen_shli_i32(tmp2, tmp2, shift);
                            tcg_gen_ext16u_i32(tmp, tmp);
                            tcg_gen_andi_i32(tmp2, tmp2, 0xffff0000);
                        }
                        tcg_gen_or_i32(tmp, tmp, tmp2);
                        tcg_temp_free_i32(tmp2);
                        store_reg(s, rd, tmp);
                    } else if ((insn & 0x00200020) == 0x00200000) {
                        /
                        tmp = load_reg(s, rm);
                        shift = (insn >> 7) & 0x1f;
                        if (insn & (1 << 6)) {
                            if (shift == 0)
                                shift = 31;
                            tcg_gen_sari_i32(tmp, tmp, shift);
                        } else {
                            tcg_gen_shli_i32(tmp, tmp, shift);
                        }
                        sh = (insn >> 16) & 0x1f;
                        tmp2 = tcg_const_i32(sh);
                        if (insn & (1 << 22))
                          gen_helper_usat(tmp, tmp, tmp2);
                        else
                          gen_helper_ssat(tmp, tmp, tmp2);
                        tcg_temp_free_i32(tmp2);
                        store_reg(s, rd, tmp);
                    } else if ((insn & 0x00300fe0) == 0x00200f20) {
                        /
                        tmp = load_reg(s, rm);
                        sh = (insn >> 16) & 0x1f;
                        tmp2 = tcg_const_i32(sh);
                        if (insn & (1 << 22))
                          gen_helper_usat16(tmp, tmp, tmp2);
                        else
                          gen_helper_ssat16(tmp, tmp, tmp2);
                        tcg_temp_free_i32(tmp2);
                        store_reg(s, rd, tmp);
                    } else if ((insn & 0x00700fe0) == 0x00000fa0) {
                        /
                        tmp = load_reg(s, rn);
                        tmp2 = load_reg(s, rm);
                        tmp3 = tcg_temp_new_i32();
                        tcg_gen_ld_i32(tmp3, cpu_env, offsetof(CPUState, GE));
                        gen_helper_sel_flags(tmp, tmp3, tmp, tmp2);
                        tcg_temp_free_i32(tmp3);
                        tcg_temp_free_i32(tmp2);
                        store_reg(s, rd, tmp);
                    } else if ((insn & 0x000003e0) == 0x00000060) {
                        tmp = load_reg(s, rm);
                        shift = (insn >> 10) & 3;
                        /
                        if (shift != 0)
                            tcg_gen_rotri_i32(tmp, tmp, shift * 8);
                        op1 = (insn >> 20) & 7;
                        switch (op1) {
                        case 0: gen_sxtb16(tmp);  break;
                        case 2: gen_sxtb(tmp);    break;
                        case 3: gen_sxth(tmp);    break;
                        case 4: gen_uxtb16(tmp);  break;
                        case 6: gen_uxtb(tmp);    break;
                        case 7: gen_uxth(tmp);    break;
                        default: goto illegal_op;
                        }
                        if (rn != 15) {
                            tmp2 = load_reg(s, rn);
                            if ((op1 & 3) == 0) {
                                gen_add16(tmp, tmp2);
                            } else {
                                tcg_gen_add_i32(tmp, tmp, tmp2);
                                tcg_temp_free_i32(tmp2);
                            }
                        }
                        store_reg(s, rd, tmp);
                    } else if ((insn & 0x003f0f60) == 0x003f0f20) {
                        /
                        tmp = load_reg(s, rm);
                        if (insn & (1 << 22)) {
                            if (insn & (1 << 7)) {
                                gen_revsh(tmp);
                            } else {
                                ARCH(6T2);
                                gen_helper_rbit(tmp, tmp);
                            }
                        } else {
                            if (insn & (1 << 7))
                                gen_rev16(tmp);
                            else
                                tcg_gen_bswap32_i32(tmp, tmp);
                        }
                        store_reg(s, rd, tmp);
                    } else {
                        goto illegal_op;
                    }
                    break;
                case 2: /
                    tmp = load_reg(s, rm);
                    tmp2 = load_reg(s, rs);
                    if (insn & (1 << 20)) {
                        /
                        tmp64 = gen_muls_i64_i32(tmp, tmp2);

                        if (rd != 15) {
                            tmp = load_reg(s, rd);
                            if (insn & (1 << 6)) {
                                tmp64 = gen_subq_msw(tmp64, tmp);
                            } else {
                                tmp64 = gen_addq_msw(tmp64, tmp);
                            }
                        }
                        if (insn & (1 << 5)) {
                            tcg_gen_addi_i64(tmp64, tmp64, 0x80000000u);
                        }
                        tcg_gen_shri_i64(tmp64, tmp64, 32);
                        tmp = tcg_temp_new_i32();
                        tcg_gen_trunc_i64_i32(tmp, tmp64);
                        tcg_temp_free_i64(tmp64);
                        store_reg(s, rn, tmp);
                    } else {
                        if (insn & (1 << 5))
                            gen_swap_half(tmp2);
                        gen_smul_dual(tmp, tmp2);
                        /
                        if (insn & (1 << 6)) {
                            tcg_gen_sub_i32(tmp, tmp, tmp2);
                        } else {
                            tcg_gen_add_i32(tmp, tmp, tmp2);
                        }
                        tcg_temp_free_i32(tmp2);
                        if (insn & (1 << 22)) {
                            /
                            tmp64 = tcg_temp_new_i64();
                            tcg_gen_ext_i32_i64(tmp64, tmp);
                            tcg_temp_free_i32(tmp);
                            gen_addq(s, tmp64, rd, rn);
                            gen_storeq_reg(s, rd, rn, tmp64);
                            tcg_temp_free_i64(tmp64);
                        } else {
                            /
                            if (rd != 15)
                              {
                                tmp2 = load_reg(s, rd);
                                gen_helper_add_setq(tmp, tmp, tmp2);
                                tcg_temp_free_i32(tmp2);
                              }
                            store_reg(s, rn, tmp);
                        }
                    }
                    break;
                case 3:
                    op1 = ((insn >> 17) & 0x38) | ((insn >> 5) & 7);
                    switch (op1) {
                    case 0: /
                        ARCH(6);
                        tmp = load_reg(s, rm);
                        tmp2 = load_reg(s, rs);
                        gen_helper_usad8(tmp, tmp, tmp2);
                        tcg_temp_free_i32(tmp2);
                        if (rd != 15) {
                            tmp2 = load_reg(s, rd);
                            tcg_gen_add_i32(tmp, tmp, tmp2);
                            tcg_temp_free_i32(tmp2);
                        }
                        store_reg(s, rn, tmp);
                        break;
                    case 0x20: case 0x24: case 0x28: case 0x2c:
                        /
                        ARCH(6T2);
                        shift = (insn >> 7) & 0x1f;
                        i = (insn >> 16) & 0x1f;
                        i = i + 1 - shift;
                        if (rm == 15) {
                            tmp = tcg_temp_new_i32();
                            tcg_gen_movi_i32(tmp, 0);
                        } else {
                            tmp = load_reg(s, rm);
                        }
                        if (i != 32) {
                            tmp2 = load_reg(s, rd);
                            gen_bfi(tmp, tmp2, tmp, shift, (1u << i) - 1);
                            tcg_temp_free_i32(tmp2);
                        }
                        store_reg(s, rd, tmp);
                        break;
                    case 0x12: case 0x16: case 0x1a: case 0x1e: /
                    case 0x32: case 0x36: case 0x3a: case 0x3e: /
                        ARCH(6T2);
                        tmp = load_reg(s, rm);
                        shift = (insn >> 7) & 0x1f;
                        i = ((insn >> 16) & 0x1f) + 1;
                        if (shift + i > 32)
                            goto illegal_op;
                        if (i < 32) {
                            if (op1 & 0x20) {
                                gen_ubfx(tmp, shift, (1u << i) - 1);
                            } else {
                                gen_sbfx(tmp, shift, i);
                            }
                        }
                        store_reg(s, rd, tmp);
                        break;
                    default:
                        goto illegal_op;
                    }
                    break;
                }
                break;
            }
        do_ldst:
            /
            sh = (0xf << 20) | (0xf << 4);
            if (op1 == 0x7 && ((insn & sh) == sh))
            {
                goto illegal_op;
            }
            /
            rn = (insn >> 16) & 0xf;
            rd = (insn >> 12) & 0xf;
            tmp2 = load_reg(s, rn);
            i = (IS_USER(s) || (insn & 0x01200000) == 0x00200000);
            if (insn & (1 << 24))
                gen_add_data_offset(s, insn, tmp2);
            if (insn & (1 << 20)) {
                /
                if (insn & (1 << 22)) {
                    tmp = gen_ld8u(tmp2, i);
                } else {
                    tmp = gen_ld32(tmp2, i);
                }
            } else {
                /
                tmp = load_reg(s, rd);
                if (insn & (1 << 22))
                    gen_st8(tmp, tmp2, i);
                else
                    gen_st32(tmp, tmp2, i);
            }
            if (!(insn & (1 << 24))) {
                gen_add_data_offset(s, insn, tmp2);
                store_reg(s, rn, tmp2);
            } else if (insn & (1 << 21)) {
                store_reg(s, rn, tmp2);
            } else {
                tcg_temp_free_i32(tmp2);
            }
            if (insn & (1 << 20)) {
                /
                if (rd == 15)
                    gen_bx(s, tmp);
                else
                    store_reg(s, rd, tmp);
            }
            break;
        case 0x08:
        case 0x09:
            {
                int j, n, user, loaded_base;
                TCGv loaded_var;
                /
                /
                user = 0;
                if (insn & (1 << 22)) {
                    if (IS_USER(s))
                        goto illegal_op; /

                    if ((insn & (1 << 15)) == 0)
                        user = 1;
                }
                rn = (insn >> 16) & 0xf;
                addr = load_reg(s, rn);

                /
                loaded_base = 0;
                TCGV_UNUSED(loaded_var);
                n = 0;
                for(i=0;i<16;i++) {
                    if (insn & (1 << i))
                        n++;
                }
                /
                if (insn & (1 << 23)) {
                    if (insn & (1 << 24)) {
                        /
                        tcg_gen_addi_i32(addr, addr, 4);
                    } else {
                        /
                    }
                } else {
                    if (insn & (1 << 24)) {
                        /
                        tcg_gen_addi_i32(addr, addr, -(n * 4));
                    } else {
                        /
                        if (n != 1)
                        tcg_gen_addi_i32(addr, addr, -((n - 1) * 4));
                    }
                }
                j = 0;
                for(i=0;i<16;i++) {
                    if (insn & (1 << i)) {
                        if (insn & (1 << 20)) {
                            /
                            tmp = gen_ld32(addr, IS_USER(s));
                            if (i == 15) {
                                gen_bx(s, tmp);
                            } else if (user) {
                                tmp2 = tcg_const_i32(i);
                                gen_helper_set_user_reg(tmp2, tmp);
                                tcg_temp_free_i32(tmp2);
                                tcg_temp_free_i32(tmp);
                            } else if (i == rn) {
                                loaded_var = tmp;
                                loaded_base = 1;
                            } else {
                                store_reg(s, i, tmp);
                            }
                        } else {
                            /
                            if (i == 15) {
                                /
                                val = (long)s->pc + 4;
                                tmp = tcg_temp_new_i32();
                                tcg_gen_movi_i32(tmp, val);
                            } else if (user) {
                                tmp = tcg_temp_new_i32();
                                tmp2 = tcg_const_i32(i);
                                gen_helper_get_user_reg(tmp, tmp2);
                                tcg_temp_free_i32(tmp2);
                            } else {
                                tmp = load_reg(s, i);
                            }
                            gen_st32(tmp, addr, IS_USER(s));
                        }
                        j++;
                        /
                        if (j != n)
                            tcg_gen_addi_i32(addr, addr, 4);
                    }
                }
                if (insn & (1 << 21)) {
                    /
                    if (insn & (1 << 23)) {
                        if (insn & (1 << 24)) {
                            /
                        } else {
                            /
                            tcg_gen_addi_i32(addr, addr, 4);
                        }
                    } else {
                        if (insn & (1 << 24)) {
                            /
                            if (n != 1)
                                tcg_gen_addi_i32(addr, addr, -((n - 1) * 4));
                        } else {
                            /
                            tcg_gen_addi_i32(addr, addr, -(n * 4));
                        }
                    }
                    store_reg(s, rn, addr);
                } else {
                    tcg_temp_free_i32(addr);
                }
                if (loaded_base) {
                    store_reg(s, rn, loaded_var);
                }
                if ((insn & (1 << 22)) && !user) {
                    /
                    tmp = load_cpu_field(spsr);
                    gen_set_cpsr(tmp, 0xffffffff);
                    tcg_temp_free_i32(tmp);
                    s->is_jmp = DISAS_UPDATE;
                }
            }
            break;
        case 0xa:
        case 0xb:
            {
                int32_t offset;

                /
                val = (int32_t)s->pc;
                if (insn & (1 << 24)) {
                    tmp = tcg_temp_new_i32();
                    tcg_gen_movi_i32(tmp, val);
                    store_reg(s, 14, tmp);
                }
                offset = (((int32_t)insn << 8) >> 8);
                val += (offset << 2) + 4;
                gen_jmp(s, val);
            }
            break;
        case 0xc:
        case 0xd:
        case 0xe:
            /
            if (disas_coproc_insn(env, s, insn))
                goto illegal_op;
            break;
        case 0xf:
            /
            gen_set_pc_im(s->pc);
            s->is_jmp = DISAS_SWI;
            break;
        default:
        illegal_op:
            gen_exception_insn(s, 4, EXCP_UDEF);
            break;
        }
    }
}
