static int disas_thumb2_insn(CPUState *env, DisasContext *s, uint16_t insn_hw1)
{
    uint32_t insn, imm, shift, offset;
    uint32_t rd, rn, rm, rs;
    TCGv tmp;
    TCGv tmp2;
    TCGv tmp3;
    TCGv addr;
    TCGv_i64 tmp64;
    int op;
    int shiftop;
    int conds;
    int logic_cc;

    if (!(arm_feature(env, ARM_FEATURE_THUMB2)
          || arm_feature (env, ARM_FEATURE_M))) {
        /
        insn = insn_hw1;
        if ((insn & (1 << 12)) == 0) {
            /
            offset = ((insn & 0x7ff) << 1);
            tmp = load_reg(s, 14);
            tcg_gen_addi_i32(tmp, tmp, offset);
            tcg_gen_andi_i32(tmp, tmp, 0xfffffffc);

            tmp2 = tcg_temp_new_i32();
            tcg_gen_movi_i32(tmp2, s->pc | 1);
            store_reg(s, 14, tmp2);
            gen_bx(s, tmp);
            return 0;
        }
        if (insn & (1 << 11)) {
            /
            offset = ((insn & 0x7ff) << 1) | 1;
            tmp = load_reg(s, 14);
            tcg_gen_addi_i32(tmp, tmp, offset);

            tmp2 = tcg_temp_new_i32();
            tcg_gen_movi_i32(tmp2, s->pc | 1);
            store_reg(s, 14, tmp2);
            gen_bx(s, tmp);
            return 0;
        }
        if ((s->pc & ~TARGET_PAGE_MASK) == 0) {
            /
            offset = ((int32_t)insn << 21) >> 9;
            tcg_gen_movi_i32(cpu_R[14], s->pc + 2 + offset);
            return 0;
        }
        /
    }

    insn = lduw_code(s->pc);
    s->pc += 2;
    insn |= (uint32_t)insn_hw1 << 16;

    if ((insn & 0xf800e800) != 0xf000e800) {
        ARCH(6T2);
    }

    rn = (insn >> 16) & 0xf;
    rs = (insn >> 12) & 0xf;
    rd = (insn >> 8) & 0xf;
    rm = insn & 0xf;
    switch ((insn >> 25) & 0xf) {
    case 0: case 1: case 2: case 3:
        /
        abort();
    case 4:
        if (insn & (1 << 22)) {
            /
            if (insn & 0x01200000) {
                /
                if (rn == 15) {
                    addr = tcg_temp_new_i32();
                    tcg_gen_movi_i32(addr, s->pc & ~3);
                } else {
                    addr = load_reg(s, rn);
                }
                offset = (insn & 0xff) * 4;
                if ((insn & (1 << 23)) == 0)
                    offset = -offset;
                if (insn & (1 << 24)) {
                    tcg_gen_addi_i32(addr, addr, offset);
                    offset = 0;
                }
                if (insn & (1 << 20)) {
                    /
                    tmp = gen_ld32(addr, IS_USER(s));
                    store_reg(s, rs, tmp);
                    tcg_gen_addi_i32(addr, addr, 4);
                    tmp = gen_ld32(addr, IS_USER(s));
                    store_reg(s, rd, tmp);
                } else {
                    /
                    tmp = load_reg(s, rs);
                    gen_st32(tmp, addr, IS_USER(s));
                    tcg_gen_addi_i32(addr, addr, 4);
                    tmp = load_reg(s, rd);
                    gen_st32(tmp, addr, IS_USER(s));
                }
                if (insn & (1 << 21)) {
                    /
                    if (rn == 15)
                        goto illegal_op;
                    tcg_gen_addi_i32(addr, addr, offset - 4);
                    store_reg(s, rn, addr);
                } else {
                    tcg_temp_free_i32(addr);
                }
            } else if ((insn & (1 << 23)) == 0) {
                /
                addr = tcg_temp_local_new();
                load_reg_var(s, addr, rn);
                tcg_gen_addi_i32(addr, addr, (insn & 0xff) << 2);
                if (insn & (1 << 20)) {
                    gen_load_exclusive(s, rs, 15, addr, 2);
                } else {
                    gen_store_exclusive(s, rd, rs, 15, addr, 2);
                }
                tcg_temp_free(addr);
            } else if ((insn & (1 << 6)) == 0) {
                /
                if (rn == 15) {
                    addr = tcg_temp_new_i32();
                    tcg_gen_movi_i32(addr, s->pc);
                } else {
                    addr = load_reg(s, rn);
                }
                tmp = load_reg(s, rm);
                tcg_gen_add_i32(addr, addr, tmp);
                if (insn & (1 << 4)) {
                    /
                    tcg_gen_add_i32(addr, addr, tmp);
                    tcg_temp_free_i32(tmp);
                    tmp = gen_ld16u(addr, IS_USER(s));
                } else { /
                    tcg_temp_free_i32(tmp);
                    tmp = gen_ld8u(addr, IS_USER(s));
                }
                tcg_temp_free_i32(addr);
                tcg_gen_shli_i32(tmp, tmp, 1);
                tcg_gen_addi_i32(tmp, tmp, s->pc);
                store_reg(s, 15, tmp);
            } else {
                /
                ARCH(7);
                op = (insn >> 4) & 0x3;
                if (op == 2) {
                    goto illegal_op;
                }
                addr = tcg_temp_local_new();
                load_reg_var(s, addr, rn);
                if (insn & (1 << 20)) {
                    gen_load_exclusive(s, rs, rd, addr, op);
                } else {
                    gen_store_exclusive(s, rm, rs, rd, addr, op);
                }
                tcg_temp_free(addr);
            }
        } else {
            /
            if (((insn >> 23) & 1) == ((insn >> 24) & 1)) {
                /
                if (IS_USER(s))
                    goto illegal_op;
                if (insn & (1 << 20)) {
                    /
                    addr = load_reg(s, rn);
                    if ((insn & (1 << 24)) == 0)
                        tcg_gen_addi_i32(addr, addr, -8);
                    /
                    tmp = gen_ld32(addr, 0);
                    tcg_gen_addi_i32(addr, addr, 4);
                    tmp2 = gen_ld32(addr, 0);
                    if (insn & (1 << 21)) {
                        /
                        if (insn & (1 << 24)) {
                            tcg_gen_addi_i32(addr, addr, 4);
                        } else {
                            tcg_gen_addi_i32(addr, addr, -4);
                        }
                        store_reg(s, rn, addr);
                    } else {
                        tcg_temp_free_i32(addr);
                    }
                    gen_rfe(s, tmp, tmp2);
                } else {
                    /
                    op = (insn & 0x1f);
                    addr = tcg_temp_new_i32();
                    tmp = tcg_const_i32(op);
                    gen_helper_get_r13_banked(addr, cpu_env, tmp);
                    tcg_temp_free_i32(tmp);
                    if ((insn & (1 << 24)) == 0) {
                        tcg_gen_addi_i32(addr, addr, -8);
                    }
                    tmp = load_reg(s, 14);
                    gen_st32(tmp, addr, 0);
                    tcg_gen_addi_i32(addr, addr, 4);
                    tmp = tcg_temp_new_i32();
                    gen_helper_cpsr_read(tmp);
                    gen_st32(tmp, addr, 0);
                    if (insn & (1 << 21)) {
                        if ((insn & (1 << 24)) == 0) {
                            tcg_gen_addi_i32(addr, addr, -4);
                        } else {
                            tcg_gen_addi_i32(addr, addr, 4);
                        }
                        tmp = tcg_const_i32(op);
                        gen_helper_set_r13_banked(cpu_env, tmp, addr);
                        tcg_temp_free_i32(tmp);
                    } else {
                        tcg_temp_free_i32(addr);
                    }
                }
            } else {
                int i;
                /
                addr = load_reg(s, rn);
                offset = 0;
                for (i = 0; i < 16; i++) {
                    if (insn & (1 << i))
                        offset += 4;
                }
                if (insn & (1 << 24)) {
                    tcg_gen_addi_i32(addr, addr, -offset);
                }

                for (i = 0; i < 16; i++) {
                    if ((insn & (1 << i)) == 0)
                        continue;
                    if (insn & (1 << 20)) {
                        /
                        tmp = gen_ld32(addr, IS_USER(s));
                        if (i == 15) {
                            gen_bx(s, tmp);
                        } else {
                            store_reg(s, i, tmp);
                        }
                    } else {
                        /
                        tmp = load_reg(s, i);
                        gen_st32(tmp, addr, IS_USER(s));
                    }
                    tcg_gen_addi_i32(addr, addr, 4);
                }
                if (insn & (1 << 21)) {
                    /
                    if (insn & (1 << 24)) {
                        tcg_gen_addi_i32(addr, addr, -offset);
                    }
                    /
                    if (insn & (1 << rn))
                        goto illegal_op;
                    store_reg(s, rn, addr);
                } else {
                    tcg_temp_free_i32(addr);
                }
            }
        }
        break;
    case 5:

        op = (insn >> 21) & 0xf;
        if (op == 6) {
            /
            tmp = load_reg(s, rn);
            tmp2 = load_reg(s, rm);
            shift = ((insn >> 10) & 0x1c) | ((insn >> 6) & 0x3);
            if (insn & (1 << 5)) {
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
        } else {
            /
            if (rn == 15) {
                tmp = tcg_temp_new_i32();
                tcg_gen_movi_i32(tmp, 0);
            } else {
                tmp = load_reg(s, rn);
            }
            tmp2 = load_reg(s, rm);

            shiftop = (insn >> 4) & 3;
            shift = ((insn >> 6) & 3) | ((insn >> 10) & 0x1c);
            conds = (insn & (1 << 20)) != 0;
            logic_cc = (conds && thumb2_logic_op(op));
            gen_arm_shift_im(tmp2, shiftop, shift, logic_cc);
            if (gen_thumb2_data_op(s, op, conds, 0, tmp, tmp2))
                goto illegal_op;
            tcg_temp_free_i32(tmp2);
            if (rd != 15) {
                store_reg(s, rd, tmp);
            } else {
                tcg_temp_free_i32(tmp);
            }
        }
        break;
    case 13: /
        op = ((insn >> 22) & 6) | ((insn >> 7) & 1);
        if (op < 4 && (insn & 0xf000) != 0xf000)
            goto illegal_op;
        switch (op) {
        case 0: /
            tmp = load_reg(s, rn);
            tmp2 = load_reg(s, rm);
            if ((insn & 0x70) != 0)
                goto illegal_op;
            op = (insn >> 21) & 3;
            logic_cc = (insn & (1 << 20)) != 0;
            gen_arm_shift_reg(tmp, op, tmp2, logic_cc);
            if (logic_cc)
                gen_logic_CC(tmp);
            store_reg_bx(env, s, rd, tmp);
            break;
        case 1: /
            tmp = load_reg(s, rm);
            shift = (insn >> 4) & 3;
            /
            if (shift != 0)
                tcg_gen_rotri_i32(tmp, tmp, shift * 8);
            op = (insn >> 20) & 7;
            switch (op) {
            case 0: gen_sxth(tmp);   break;
            case 1: gen_uxth(tmp);   break;
            case 2: gen_sxtb16(tmp); break;
            case 3: gen_uxtb16(tmp); break;
            case 4: gen_sxtb(tmp);   break;
            case 5: gen_uxtb(tmp);   break;
            default: goto illegal_op;
            }
            if (rn != 15) {
                tmp2 = load_reg(s, rn);
                if ((op >> 1) == 1) {
                    gen_add16(tmp, tmp2);
                } else {
                    tcg_gen_add_i32(tmp, tmp, tmp2);
                    tcg_temp_free_i32(tmp2);
                }
            }
            store_reg(s, rd, tmp);
            break;
        case 2: /
            op = (insn >> 20) & 7;
            shift = (insn >> 4) & 7;
            if ((op & 3) == 3 || (shift & 3) == 3)
                goto illegal_op;
            tmp = load_reg(s, rn);
            tmp2 = load_reg(s, rm);
            gen_thumb2_parallel_addsub(op, shift, tmp, tmp2);
            tcg_temp_free_i32(tmp2);
            store_reg(s, rd, tmp);
            break;
        case 3: /
            op = ((insn >> 17) & 0x38) | ((insn >> 4) & 7);
            if (op < 4) {
                /
                tmp = load_reg(s, rn);
                tmp2 = load_reg(s, rm);
                if (op & 1)
                    gen_helper_double_saturate(tmp, tmp);
                if (op & 2)
                    gen_helper_sub_saturate(tmp, tmp2, tmp);
                else
                    gen_helper_add_saturate(tmp, tmp, tmp2);
                tcg_temp_free_i32(tmp2);
            } else {
                tmp = load_reg(s, rn);
                switch (op) {
                case 0x0a: /
                    gen_helper_rbit(tmp, tmp);
                    break;
                case 0x08: /
                    tcg_gen_bswap32_i32(tmp, tmp);
                    break;
                case 0x09: /
                    gen_rev16(tmp);
                    break;
                case 0x0b: /
                    gen_revsh(tmp);
                    break;
                case 0x10: /
                    tmp2 = load_reg(s, rm);
                    tmp3 = tcg_temp_new_i32();
                    tcg_gen_ld_i32(tmp3, cpu_env, offsetof(CPUState, GE));
                    gen_helper_sel_flags(tmp, tmp3, tmp, tmp2);
                    tcg_temp_free_i32(tmp3);
                    tcg_temp_free_i32(tmp2);
                    break;
                case 0x18: /
                    gen_helper_clz(tmp, tmp);
                    break;
                default:
                    goto illegal_op;
                }
            }
            store_reg(s, rd, tmp);
            break;
        case 4: case 5: /
            op = (insn >> 4) & 0xf;
            tmp = load_reg(s, rn);
            tmp2 = load_reg(s, rm);
            switch ((insn >> 20) & 7) {
            case 0: /
                tcg_gen_mul_i32(tmp, tmp, tmp2);
                tcg_temp_free_i32(tmp2);
                if (rs != 15) {
                    tmp2 = load_reg(s, rs);
                    if (op)
                        tcg_gen_sub_i32(tmp, tmp2, tmp);
                    else
                        tcg_gen_add_i32(tmp, tmp, tmp2);
                    tcg_temp_free_i32(tmp2);
                }
                break;
            case 1: /
                gen_mulxy(tmp, tmp2, op & 2, op & 1);
                tcg_temp_free_i32(tmp2);
                if (rs != 15) {
                    tmp2 = load_reg(s, rs);
                    gen_helper_add_setq(tmp, tmp, tmp2);
                    tcg_temp_free_i32(tmp2);
                }
                break;
            case 2: /
            case 4: /
                if (op)
                    gen_swap_half(tmp2);
                gen_smul_dual(tmp, tmp2);
                /
                if (insn & (1 << 22)) {
                    tcg_gen_sub_i32(tmp, tmp, tmp2);
                } else {
                    tcg_gen_add_i32(tmp, tmp, tmp2);
                }
                tcg_temp_free_i32(tmp2);
                if (rs != 15)
                  {
                    tmp2 = load_reg(s, rs);
                    gen_helper_add_setq(tmp, tmp, tmp2);
                    tcg_temp_free_i32(tmp2);
                  }
                break;
            case 3: /
                if (op)
                    tcg_gen_sari_i32(tmp2, tmp2, 16);
                else
                    gen_sxth(tmp2);
                tmp64 = gen_muls_i64_i32(tmp, tmp2);
                tcg_gen_shri_i64(tmp64, tmp64, 16);
                tmp = tcg_temp_new_i32();
                tcg_gen_trunc_i64_i32(tmp, tmp64);
                tcg_temp_free_i64(tmp64);
                if (rs != 15)
                  {
                    tmp2 = load_reg(s, rs);
                    gen_helper_add_setq(tmp, tmp, tmp2);
                    tcg_temp_free_i32(tmp2);
                  }
                break;
            case 5: case 6: /
                tmp64 = gen_muls_i64_i32(tmp, tmp2);
                if (rs != 15) {
                    tmp = load_reg(s, rs);
                    if (insn & (1 << 20)) {
                        tmp64 = gen_addq_msw(tmp64, tmp);
                    } else {
                        tmp64 = gen_subq_msw(tmp64, tmp);
                    }
                }
                if (insn & (1 << 4)) {
                    tcg_gen_addi_i64(tmp64, tmp64, 0x80000000u);
                }
                tcg_gen_shri_i64(tmp64, tmp64, 32);
                tmp = tcg_temp_new_i32();
                tcg_gen_trunc_i64_i32(tmp, tmp64);
                tcg_temp_free_i64(tmp64);
                break;
            case 7: /
                gen_helper_usad8(tmp, tmp, tmp2);
                tcg_temp_free_i32(tmp2);
                if (rs != 15) {
                    tmp2 = load_reg(s, rs);
                    tcg_gen_add_i32(tmp, tmp, tmp2);
                    tcg_temp_free_i32(tmp2);
                }
                break;
            }
            store_reg(s, rd, tmp);
            break;
        case 6: case 7: /
            op = ((insn >> 4) & 0xf) | ((insn >> 16) & 0x70);
            tmp = load_reg(s, rn);
            tmp2 = load_reg(s, rm);
            if ((op & 0x50) == 0x10) {
                /
                if (!arm_feature(env, ARM_FEATURE_DIV))
                    goto illegal_op;
                if (op & 0x20)
                    gen_helper_udiv(tmp, tmp, tmp2);
                else
                    gen_helper_sdiv(tmp, tmp, tmp2);
                tcg_temp_free_i32(tmp2);
                store_reg(s, rd, tmp);
            } else if ((op & 0xe) == 0xc) {
                /
                if (op & 1)
                    gen_swap_half(tmp2);
                gen_smul_dual(tmp, tmp2);
                if (op & 0x10) {
                    tcg_gen_sub_i32(tmp, tmp, tmp2);
                } else {
                    tcg_gen_add_i32(tmp, tmp, tmp2);
                }
                tcg_temp_free_i32(tmp2);
                /
                tmp64 = tcg_temp_new_i64();
                tcg_gen_ext_i32_i64(tmp64, tmp);
                tcg_temp_free_i32(tmp);
                gen_addq(s, tmp64, rs, rd);
                gen_storeq_reg(s, rs, rd, tmp64);
                tcg_temp_free_i64(tmp64);
            } else {
                if (op & 0x20) {
                    /
                    tmp64 = gen_mulu_i64_i32(tmp, tmp2);
                } else {
                    if (op & 8) {
                        /
                        gen_mulxy(tmp, tmp2, op & 2, op & 1);
                        tcg_temp_free_i32(tmp2);
                        tmp64 = tcg_temp_new_i64();
                        tcg_gen_ext_i32_i64(tmp64, tmp);
                        tcg_temp_free_i32(tmp);
                    } else {
                        /
                        tmp64 = gen_muls_i64_i32(tmp, tmp2);
                    }
                }
                if (op & 4) {
                    /
                    gen_addq_lo(s, tmp64, rs);
                    gen_addq_lo(s, tmp64, rd);
                } else if (op & 0x40) {
                    /
                    gen_addq(s, tmp64, rs, rd);
                }
                gen_storeq_reg(s, rs, rd, tmp64);
                tcg_temp_free_i64(tmp64);
            }
            break;
        }
        break;
    case 6: case 7: case 14: case 15:
        /
        if (((insn >> 24) & 3) == 3) {
            /
            insn = (insn & 0xe2ffffff) | ((insn & (1 << 28)) >> 4) | (1 << 28);
            if (disas_neon_data_insn(env, s, insn))
                goto illegal_op;
        } else {
            if (insn & (1 << 28))
                goto illegal_op;
            if (disas_coproc_insn (env, s, insn))
                goto illegal_op;
        }
        break;
    case 8: case 9: case 10: case 11:
        if (insn & (1 << 15)) {
            /
            if (insn & 0x5000) {
                /
                /
                offset = ((int32_t)insn << 5) >> 9 & ~(int32_t)0xfff;
                /
                offset |= (insn & 0x7ff) << 1;
                /
                offset ^= ((~insn) & (1 << 13)) << 10;
                offset ^= ((~insn) & (1 << 11)) << 11;

                if (insn & (1 << 14)) {
                    /
                    tcg_gen_movi_i32(cpu_R[14], s->pc | 1);
                }

                offset += s->pc;
                if (insn & (1 << 12)) {
                    /
                    gen_jmp(s, offset);
                } else {
                    /
                    offset &= ~(uint32_t)2;
                    gen_bx_im(s, offset);
                }
            } else if (((insn >> 23) & 7) == 7) {
                /
                if (insn & (1 << 13))
                    goto illegal_op;

                if (insn & (1 << 26)) {
                    /
                    goto illegal_op; /
                } else {
                    op = (insn >> 20) & 7;
                    switch (op) {
                    case 0: /
                        if (IS_M(env)) {
                            tmp = load_reg(s, rn);
                            addr = tcg_const_i32(insn & 0xff);
                            gen_helper_v7m_msr(cpu_env, addr, tmp);
                            tcg_temp_free_i32(addr);
                            tcg_temp_free_i32(tmp);
                            gen_lookup_tb(s);
                            break;
                        }
                        /
                    case 1: /
                        if (IS_M(env))
                            goto illegal_op;
                        tmp = load_reg(s, rn);
                        if (gen_set_psr(s,
                              msr_mask(env, s, (insn >> 8) & 0xf, op == 1),
                              op == 1, tmp))
                            goto illegal_op;
                        break;
                    case 2: /
                        if (((insn >> 8) & 7) == 0) {
                            gen_nop_hint(s, insn & 0xff);
                        }
                        /
                        if (IS_USER(s))
                            break;
                        offset = 0;
                        imm = 0;
                        if (insn & (1 << 10)) {
                            if (insn & (1 << 7))
                                offset |= CPSR_A;
                            if (insn & (1 << 6))
                                offset |= CPSR_I;
                            if (insn & (1 << 5))
                                offset |= CPSR_F;
                            if (insn & (1 << 9))
                                imm = CPSR_A | CPSR_I | CPSR_F;
                        }
                        if (insn & (1 << 8)) {
                            offset |= 0x1f;
                            imm |= (insn & 0x1f);
                        }
                        if (offset) {
                            gen_set_psr_im(s, offset, 0, imm);
                        }
                        break;
                    case 3: /
                        ARCH(7);
                        op = (insn >> 4) & 0xf;
                        switch (op) {
                        case 2: /
                            gen_clrex(s);
                            break;
                        case 4: /
                        case 5: /
                        case 6: /
                            /
                            break;
                        default:
                            goto illegal_op;
                        }
                        break;
                    case 4: /
                        /
                        tmp = load_reg(s, rn);
                        gen_bx(s, tmp);
                        break;
                    case 5: /
                        if (IS_USER(s)) {
                            goto illegal_op;
                        }
                        if (rn != 14 || rd != 15) {
                            goto illegal_op;
                        }
                        tmp = load_reg(s, rn);
                        tcg_gen_subi_i32(tmp, tmp, insn & 0xff);
                        gen_exception_return(s, tmp);
                        break;
                    case 6: /
                        tmp = tcg_temp_new_i32();
                        if (IS_M(env)) {
                            addr = tcg_const_i32(insn & 0xff);
                            gen_helper_v7m_mrs(tmp, cpu_env, addr);
                            tcg_temp_free_i32(addr);
                        } else {
                            gen_helper_cpsr_read(tmp);
                        }
                        store_reg(s, rd, tmp);
                        break;
                    case 7: /
                        /
                        if (IS_USER(s) || IS_M(env))
                            goto illegal_op;
                        tmp = load_cpu_field(spsr);
                        store_reg(s, rd, tmp);
                        break;
                    }
                }
            } else {
                /
                op = (insn >> 22) & 0xf;
                /
                s->condlabel = gen_new_label();
                gen_test_cc(op ^ 1, s->condlabel);
                s->condjmp = 1;

                /
                offset = (insn & 0x7ff) << 1;
                /
                offset |= (insn & 0x003f0000) >> 4;
                /
                offset |= ((int32_t)((insn << 5) & 0x80000000)) >> 11;
                /
                offset |= (insn & (1 << 13)) << 5;
                /
                offset |= (insn & (1 << 11)) << 8;

                /
                gen_jmp(s, s->pc + offset);
            }
        } else {
            /
            if (insn & (1 << 25)) {
                if (insn & (1 << 24)) {
                    if (insn & (1 << 20))
                        goto illegal_op;
                    /
                    op = (insn >> 21) & 7;
                    imm = insn & 0x1f;
                    shift = ((insn >> 6) & 3) | ((insn >> 10) & 0x1c);
                    if (rn == 15) {
                        tmp = tcg_temp_new_i32();
                        tcg_gen_movi_i32(tmp, 0);
                    } else {
                        tmp = load_reg(s, rn);
                    }
                    switch (op) {
                    case 2: /
                        imm++;
                        if (shift + imm > 32)
                            goto illegal_op;
                        if (imm < 32)
                            gen_sbfx(tmp, shift, imm);
                        break;
                    case 6: /
                        imm++;
                        if (shift + imm > 32)
                            goto illegal_op;
                        if (imm < 32)
                            gen_ubfx(tmp, shift, (1u << imm) - 1);
                        break;
                    case 3: /
                        if (imm < shift)
                            goto illegal_op;
                        imm = imm + 1 - shift;
                        if (imm != 32) {
                            tmp2 = load_reg(s, rd);
                            gen_bfi(tmp, tmp2, tmp, shift, (1u << imm) - 1);
                            tcg_temp_free_i32(tmp2);
                        }
                        break;
                    case 7:
                        goto illegal_op;
                    default: /
                        if (shift) {
                            if (op & 1)
                                tcg_gen_sari_i32(tmp, tmp, shift);
                            else
                                tcg_gen_shli_i32(tmp, tmp, shift);
                        }
                        tmp2 = tcg_const_i32(imm);
                        if (op & 4) {
                            /
                            if ((op & 1) && shift == 0)
                                gen_helper_usat16(tmp, tmp, tmp2);
                            else
                                gen_helper_usat(tmp, tmp, tmp2);
                        } else {
                            /
                            if ((op & 1) && shift == 0)
                                gen_helper_ssat16(tmp, tmp, tmp2);
                            else
                                gen_helper_ssat(tmp, tmp, tmp2);
                        }
                        tcg_temp_free_i32(tmp2);
                        break;
                    }
                    store_reg(s, rd, tmp);
                } else {
                    imm = ((insn & 0x04000000) >> 15)
                          | ((insn & 0x7000) >> 4) | (insn & 0xff);
                    if (insn & (1 << 22)) {
                        /
                        imm |= (insn >> 4) & 0xf000;
                        if (insn & (1 << 23)) {
                            /
                            tmp = load_reg(s, rd);
                            tcg_gen_ext16u_i32(tmp, tmp);
                            tcg_gen_ori_i32(tmp, tmp, imm << 16);
                        } else {
                            /
                            tmp = tcg_temp_new_i32();
                            tcg_gen_movi_i32(tmp, imm);
                        }
                    } else {
                        /
                        if (rn == 15) {
                            offset = s->pc & ~(uint32_t)3;
                            if (insn & (1 << 23))
                                offset -= imm;
                            else
                                offset += imm;
                            tmp = tcg_temp_new_i32();
                            tcg_gen_movi_i32(tmp, offset);
                        } else {
                            tmp = load_reg(s, rn);
                            if (insn & (1 << 23))
                                tcg_gen_subi_i32(tmp, tmp, imm);
                            else
                                tcg_gen_addi_i32(tmp, tmp, imm);
                        }
                    }
                    store_reg(s, rd, tmp);
                }
            } else {
                int shifter_out = 0;
                /
                shift = ((insn & 0x04000000) >> 23) | ((insn & 0x7000) >> 12);
                imm = (insn & 0xff);
                switch (shift) {
                case 0: /
                    /
                    break;
                case 1: /
                    imm |= imm << 16;
                    break;
                case 2: /
                    imm |= imm << 16;
                    imm <<= 8;
                    break;
                case 3: /
                    imm |= imm << 16;
                    imm |= imm << 8;
                    break;
                default: /
                    shift = (shift << 1) | (imm >> 7);
                    imm |= 0x80;
                    imm = imm << (32 - shift);
                    shifter_out = 1;
                    break;
                }
                tmp2 = tcg_temp_new_i32();
                tcg_gen_movi_i32(tmp2, imm);
                rn = (insn >> 16) & 0xf;
                if (rn == 15) {
                    tmp = tcg_temp_new_i32();
                    tcg_gen_movi_i32(tmp, 0);
                } else {
                    tmp = load_reg(s, rn);
                }
                op = (insn >> 21) & 0xf;
                if (gen_thumb2_data_op(s, op, (insn & (1 << 20)) != 0,
                                       shifter_out, tmp, tmp2))
                    goto illegal_op;
                tcg_temp_free_i32(tmp2);
                rd = (insn >> 8) & 0xf;
                if (rd != 15) {
                    store_reg(s, rd, tmp);
                } else {
                    tcg_temp_free_i32(tmp);
                }
            }
        }
        break;
    case 12: /
        {
        int postinc = 0;
        int writeback = 0;
        int user;
        if ((insn & 0x01100000) == 0x01000000) {
            if (disas_neon_ls_insn(env, s, insn))
                goto illegal_op;
            break;
        }
        op = ((insn >> 21) & 3) | ((insn >> 22) & 4);
        if (rs == 15) {
            if (!(insn & (1 << 20))) {
                goto illegal_op;
            }
            if (op != 2) {
                /
                int op1 = (insn >> 23) & 3;
                int op2 = (insn >> 6) & 0x3f;
                if (op & 2) {
                    goto illegal_op;
                }
                if (rn == 15) {
                    /
                    return 0;
                }
                if (op1 & 1) {
                    return 0; /
                }
                if ((op2 == 0) || ((op2 & 0x3c) == 0x30)) {
                    return 0; /
                }
                /
                return 1;
            }
        }
        user = IS_USER(s);
        if (rn == 15) {
            addr = tcg_temp_new_i32();
            /
            /
            imm = s->pc & 0xfffffffc;
            if (insn & (1 << 23))
                imm += insn & 0xfff;
            else
                imm -= insn & 0xfff;
            tcg_gen_movi_i32(addr, imm);
        } else {
            addr = load_reg(s, rn);
            if (insn & (1 << 23)) {
                /
                imm = insn & 0xfff;
                tcg_gen_addi_i32(addr, addr, imm);
            } else {
                imm = insn & 0xff;
                switch ((insn >> 8) & 0xf) {
                case 0x0: /
                    shift = (insn >> 4) & 0xf;
                    if (shift > 3) {
                        tcg_temp_free_i32(addr);
                        goto illegal_op;
                    }
                    tmp = load_reg(s, rm);
                    if (shift)
                        tcg_gen_shli_i32(tmp, tmp, shift);
                    tcg_gen_add_i32(addr, addr, tmp);
                    tcg_temp_free_i32(tmp);
                    break;
                case 0xc: /
                    tcg_gen_addi_i32(addr, addr, -imm);
                    break;
                case 0xe: /
                    tcg_gen_addi_i32(addr, addr, imm);
                    user = 1;
                    break;
                case 0x9: /
                    imm = -imm;
                    /
                case 0xb: /
                    postinc = 1;
                    writeback = 1;
                    break;
                case 0xd: /
                    imm = -imm;
                    /
                case 0xf: /
                    tcg_gen_addi_i32(addr, addr, imm);
                    writeback = 1;
                    break;
                default:
                    tcg_temp_free_i32(addr);
                    goto illegal_op;
                }
            }
        }
        if (insn & (1 << 20)) {
            /
            switch (op) {
            case 0: tmp = gen_ld8u(addr, user); break;
            case 4: tmp = gen_ld8s(addr, user); break;
            case 1: tmp = gen_ld16u(addr, user); break;
            case 5: tmp = gen_ld16s(addr, user); break;
            case 2: tmp = gen_ld32(addr, user); break;
            default:
                tcg_temp_free_i32(addr);
                goto illegal_op;
            }
            if (rs == 15) {
                gen_bx(s, tmp);
            } else {
                store_reg(s, rs, tmp);
            }
        } else {
            /
            tmp = load_reg(s, rs);
            switch (op) {
            case 0: gen_st8(tmp, addr, user); break;
            case 1: gen_st16(tmp, addr, user); break;
            case 2: gen_st32(tmp, addr, user); break;
            default:
                tcg_temp_free_i32(addr);
                goto illegal_op;
            }
        }
        if (postinc)
            tcg_gen_addi_i32(addr, addr, imm);
        if (writeback) {
            store_reg(s, rn, addr);
        } else {
            tcg_temp_free_i32(addr);
        }
        }
        break;
    default:
        goto illegal_op;
    }
    return 0;
illegal_op:
    return 1;
}
