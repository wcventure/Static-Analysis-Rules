static void dec_calc(DisasContext *dc, uint32_t insn)
{
    uint32_t op0, op1, op2;
    uint32_t ra, rb, rd;
    op0 = extract32(insn, 0, 4);
    op1 = extract32(insn, 8, 2);
    op2 = extract32(insn, 6, 2);
    ra = extract32(insn, 16, 5);
    rb = extract32(insn, 11, 5);
    rd = extract32(insn, 21, 5);

    switch (op0) {
    case 0x0000:
        switch (op1) {
        case 0x00:    /
            LOG_DIS("l.add r%d, r%d, r%d\n", rd, ra, rb);
            {
                TCGLabel *lab = gen_new_label();
                TCGv_i64 ta = tcg_temp_new_i64();
                TCGv_i64 tb = tcg_temp_new_i64();
                TCGv_i64 td = tcg_temp_local_new_i64();
                TCGv_i32 res = tcg_temp_local_new_i32();
                TCGv_i32 sr_ove = tcg_temp_local_new_i32();
                tcg_gen_extu_i32_i64(ta, cpu_R[ra]);
                tcg_gen_extu_i32_i64(tb, cpu_R[rb]);
                tcg_gen_add_i64(td, ta, tb);
                tcg_gen_extrl_i64_i32(res, td);
                tcg_gen_shri_i64(td, td, 31);
                tcg_gen_andi_i64(td, td, 0x3);
                /
                tcg_gen_brcondi_i64(TCG_COND_EQ, td, 0x0, lab);
                tcg_gen_brcondi_i64(TCG_COND_EQ, td, 0x3, lab);
                tcg_gen_ori_i32(cpu_sr, cpu_sr, (SR_OV | SR_CY));
                tcg_gen_andi_i32(sr_ove, cpu_sr, SR_OVE);
                tcg_gen_brcondi_i32(TCG_COND_NE, sr_ove, SR_OVE, lab);
                gen_exception(dc, EXCP_RANGE);
                gen_set_label(lab);
                tcg_gen_mov_i32(cpu_R[rd], res);
                tcg_temp_free_i64(ta);
                tcg_temp_free_i64(tb);
                tcg_temp_free_i64(td);
                tcg_temp_free_i32(res);
                tcg_temp_free_i32(sr_ove);
            }
            break;
        default:
            gen_illegal_exception(dc);
            break;
        }
        break;

    case 0x0001:    /
        switch (op1) {
        case 0x00:
            LOG_DIS("l.addc r%d, r%d, r%d\n", rd, ra, rb);
            {
                TCGLabel *lab = gen_new_label();
                TCGv_i64 ta = tcg_temp_new_i64();
                TCGv_i64 tb = tcg_temp_new_i64();
                TCGv_i64 tcy = tcg_temp_local_new_i64();
                TCGv_i64 td = tcg_temp_local_new_i64();
                TCGv_i32 res = tcg_temp_local_new_i32();
                TCGv_i32 sr_cy = tcg_temp_local_new_i32();
                TCGv_i32 sr_ove = tcg_temp_local_new_i32();
                tcg_gen_extu_i32_i64(ta, cpu_R[ra]);
                tcg_gen_extu_i32_i64(tb, cpu_R[rb]);
                tcg_gen_andi_i32(sr_cy, cpu_sr, SR_CY);
                tcg_gen_extu_i32_i64(tcy, sr_cy);
                tcg_gen_shri_i64(tcy, tcy, 10);
                tcg_gen_add_i64(td, ta, tb);
                tcg_gen_add_i64(td, td, tcy);
                tcg_gen_extrl_i64_i32(res, td);
                tcg_gen_shri_i64(td, td, 32);
                tcg_gen_andi_i64(td, td, 0x3);
                /
                tcg_gen_brcondi_i64(TCG_COND_EQ, td, 0x0, lab);
                tcg_gen_brcondi_i64(TCG_COND_EQ, td, 0x3, lab);
                tcg_gen_ori_i32(cpu_sr, cpu_sr, (SR_OV | SR_CY));
                tcg_gen_andi_i32(sr_ove, cpu_sr, SR_OVE);
                tcg_gen_brcondi_i32(TCG_COND_NE, sr_ove, SR_OVE, lab);
                gen_exception(dc, EXCP_RANGE);
                gen_set_label(lab);
                tcg_gen_mov_i32(cpu_R[rd], res);
                tcg_temp_free_i64(ta);
                tcg_temp_free_i64(tb);
                tcg_temp_free_i64(tcy);
                tcg_temp_free_i64(td);
                tcg_temp_free_i32(res);
                tcg_temp_free_i32(sr_cy);
                tcg_temp_free_i32(sr_ove);
            }
            break;
        default:
            gen_illegal_exception(dc);
            break;
        }
        break;

    case 0x0002:    /
        switch (op1) {
        case 0x00:
            LOG_DIS("l.sub r%d, r%d, r%d\n", rd, ra, rb);
            {
                TCGLabel *lab = gen_new_label();
                TCGv_i64 ta = tcg_temp_new_i64();
                TCGv_i64 tb = tcg_temp_new_i64();
                TCGv_i64 td = tcg_temp_local_new_i64();
                TCGv_i32 res = tcg_temp_local_new_i32();
                TCGv_i32 sr_ove = tcg_temp_local_new_i32();

                tcg_gen_extu_i32_i64(ta, cpu_R[ra]);
                tcg_gen_extu_i32_i64(tb, cpu_R[rb]);
                tcg_gen_sub_i64(td, ta, tb);
                tcg_gen_extrl_i64_i32(res, td);
                tcg_gen_shri_i64(td, td, 31);
                tcg_gen_andi_i64(td, td, 0x3);
                /
                tcg_gen_brcondi_i64(TCG_COND_EQ, td, 0x0, lab);
                tcg_gen_brcondi_i64(TCG_COND_EQ, td, 0x3, lab);
                tcg_gen_ori_i32(cpu_sr, cpu_sr, (SR_OV | SR_CY));
                tcg_gen_andi_i32(sr_ove, cpu_sr, SR_OVE);
                tcg_gen_brcondi_i32(TCG_COND_NE, sr_ove, SR_OVE, lab);
                gen_exception(dc, EXCP_RANGE);
                gen_set_label(lab);
                tcg_gen_mov_i32(cpu_R[rd], res);
                tcg_temp_free_i64(ta);
                tcg_temp_free_i64(tb);
                tcg_temp_free_i64(td);
                tcg_temp_free_i32(res);
                tcg_temp_free_i32(sr_ove);
            }
            break;
        default:
            gen_illegal_exception(dc);
            break;
        }
        break;

    case 0x0003:    /
        switch (op1) {
        case 0x00:
            LOG_DIS("l.and r%d, r%d, r%d\n", rd, ra, rb);
            tcg_gen_and_tl(cpu_R[rd], cpu_R[ra], cpu_R[rb]);
            break;
        default:
            gen_illegal_exception(dc);
            break;
        }
        break;

    case 0x0004:    /
        switch (op1) {
        case 0x00:
            LOG_DIS("l.or r%d, r%d, r%d\n", rd, ra, rb);
            tcg_gen_or_tl(cpu_R[rd], cpu_R[ra], cpu_R[rb]);
            break;
        default:
            gen_illegal_exception(dc);
            break;
        }
        break;

    case 0x0005:
        switch (op1) {
        case 0x00:    /
            LOG_DIS("l.xor r%d, r%d, r%d\n", rd, ra, rb);
            tcg_gen_xor_tl(cpu_R[rd], cpu_R[ra], cpu_R[rb]);
            break;
        default:
            gen_illegal_exception(dc);
            break;
        }
        break;

    case 0x0006:
        switch (op1) {
        case 0x03:    /
            LOG_DIS("l.mul r%d, r%d, r%d\n", rd, ra, rb);
            if (ra != 0 && rb != 0) {
                gen_helper_mul32(cpu_R[rd], cpu_env, cpu_R[ra], cpu_R[rb]);
            } else {
                tcg_gen_movi_tl(cpu_R[rd], 0x0);
            }
            break;
        default:
            gen_illegal_exception(dc);
            break;
        }
        break;

    case 0x0009:
        switch (op1) {
        case 0x03:    /
            LOG_DIS("l.div r%d, r%d, r%d\n", rd, ra, rb);
            {
                TCGLabel *lab0 = gen_new_label();
                TCGLabel *lab1 = gen_new_label();
                TCGLabel *lab2 = gen_new_label();
                TCGLabel *lab3 = gen_new_label();
                TCGv_i32 sr_ove = tcg_temp_local_new_i32();
                if (rb == 0) {
                    tcg_gen_ori_tl(cpu_sr, cpu_sr, (SR_OV | SR_CY));
                    tcg_gen_andi_tl(sr_ove, cpu_sr, SR_OVE);
                    tcg_gen_brcondi_tl(TCG_COND_NE, sr_ove, SR_OVE, lab0);
                    gen_exception(dc, EXCP_RANGE);
                    gen_set_label(lab0);
                } else {
                    tcg_gen_brcondi_tl(TCG_COND_EQ, cpu_R[rb],
                                       0x00000000, lab1);
                    tcg_gen_brcondi_tl(TCG_COND_NE, cpu_R[ra],
                                       0x80000000, lab2);
                    tcg_gen_brcondi_tl(TCG_COND_NE, cpu_R[rb],
                                       0xffffffff, lab2);
                    gen_set_label(lab1);
                    tcg_gen_ori_tl(cpu_sr, cpu_sr, (SR_OV | SR_CY));
                    tcg_gen_andi_tl(sr_ove, cpu_sr, SR_OVE);
                    tcg_gen_brcondi_tl(TCG_COND_NE, sr_ove, SR_OVE, lab3);
                    gen_exception(dc, EXCP_RANGE);
                    gen_set_label(lab2);
                    tcg_gen_div_tl(cpu_R[rd], cpu_R[ra], cpu_R[rb]);
                    gen_set_label(lab3);
                }
                tcg_temp_free_i32(sr_ove);
            }
            break;

        default:
            gen_illegal_exception(dc);
            break;
        }
        break;

    case 0x000a:
        switch (op1) {
        case 0x03:    /
            LOG_DIS("l.divu r%d, r%d, r%d\n", rd, ra, rb);
            {
                TCGLabel *lab0 = gen_new_label();
                TCGLabel *lab1 = gen_new_label();
                TCGLabel *lab2 = gen_new_label();
                TCGv_i32 sr_ove = tcg_temp_local_new_i32();
                if (rb == 0) {
                    tcg_gen_ori_tl(cpu_sr, cpu_sr, (SR_OV | SR_CY));
                    tcg_gen_andi_tl(sr_ove, cpu_sr, SR_OVE);
                    tcg_gen_brcondi_tl(TCG_COND_NE, sr_ove, SR_OVE, lab0);
                    gen_exception(dc, EXCP_RANGE);
                    gen_set_label(lab0);
                } else {
                    tcg_gen_brcondi_tl(TCG_COND_NE, cpu_R[rb],
                                       0x00000000, lab1);
                    tcg_gen_ori_tl(cpu_sr, cpu_sr, (SR_OV | SR_CY));
                    tcg_gen_andi_tl(sr_ove, cpu_sr, SR_OVE);
                    tcg_gen_brcondi_tl(TCG_COND_NE, sr_ove, SR_OVE, lab2);
                    gen_exception(dc, EXCP_RANGE);
                    gen_set_label(lab1);
                    tcg_gen_divu_tl(cpu_R[rd], cpu_R[ra], cpu_R[rb]);
                    gen_set_label(lab2);
                }
                tcg_temp_free_i32(sr_ove);
            }
            break;

        default:
            gen_illegal_exception(dc);
            break;
        }
        break;

    case 0x000b:
        switch (op1) {
        case 0x03:    /
            LOG_DIS("l.mulu r%d, r%d, r%d\n", rd, ra, rb);
            if (rb != 0 && ra != 0) {
                TCGv_i64 result = tcg_temp_local_new_i64();
                TCGv_i64 tra = tcg_temp_local_new_i64();
                TCGv_i64 trb = tcg_temp_local_new_i64();
                TCGv_i64 high = tcg_temp_new_i64();
                TCGv_i32 sr_ove = tcg_temp_local_new_i32();
                TCGLabel *lab = gen_new_label();
                /
                tcg_gen_extu_i32_i64(tra, cpu_R[ra]);
                tcg_gen_extu_i32_i64(trb, cpu_R[rb]);
                tcg_gen_mul_i64(result, tra, trb);
                tcg_temp_free_i64(tra);
                tcg_temp_free_i64(trb);
                tcg_gen_shri_i64(high, result, TARGET_LONG_BITS);
                /
                tcg_gen_brcondi_i64(TCG_COND_EQ, high, 0x00000000, lab);
                tcg_gen_ori_tl(cpu_sr, cpu_sr, (SR_OV | SR_CY));
                tcg_gen_andi_tl(sr_ove, cpu_sr, SR_OVE);
                tcg_gen_brcondi_tl(TCG_COND_NE, sr_ove, SR_OVE, lab);
                gen_exception(dc, EXCP_RANGE);
                gen_set_label(lab);
                tcg_temp_free_i64(high);
                tcg_gen_trunc_i64_tl(cpu_R[rd], result);
                tcg_temp_free_i64(result);
                tcg_temp_free_i32(sr_ove);
            } else {
                tcg_gen_movi_tl(cpu_R[rd], 0);
            }
            break;

        default:
            gen_illegal_exception(dc);
            break;
        }
        break;

    case 0x000e:
        switch (op1) {
        case 0x00:    /
            LOG_DIS("l.cmov r%d, r%d, r%d\n", rd, ra, rb);
            {
                TCGLabel *lab = gen_new_label();
                TCGv res = tcg_temp_local_new();
                TCGv sr_f = tcg_temp_new();
                tcg_gen_andi_tl(sr_f, cpu_sr, SR_F);
                tcg_gen_mov_tl(res, cpu_R[rb]);
                tcg_gen_brcondi_tl(TCG_COND_NE, sr_f, SR_F, lab);
                tcg_gen_mov_tl(res, cpu_R[ra]);
                gen_set_label(lab);
                tcg_gen_mov_tl(cpu_R[rd], res);
                tcg_temp_free(sr_f);
                tcg_temp_free(res);
            }
            break;

        default:
            gen_illegal_exception(dc);
            break;
        }
        break;

    case 0x000f:
        switch (op1) {
        case 0x00:    /
            LOG_DIS("l.ff1 r%d, r%d, r%d\n", rd, ra, rb);
            tcg_gen_ctzi_tl(cpu_R[rd], cpu_R[ra], -1);
            tcg_gen_addi_tl(cpu_R[rd], cpu_R[rd], 1);
            break;
        case 0x01:    /
            LOG_DIS("l.fl1 r%d, r%d, r%d\n", rd, ra, rb);
            tcg_gen_clzi_tl(cpu_R[rd], cpu_R[ra], TARGET_LONG_BITS);
            tcg_gen_subfi_tl(cpu_R[rd], TARGET_LONG_BITS, cpu_R[rd]);
            break;

        default:
            gen_illegal_exception(dc);
            break;
        }
        break;

    case 0x0008:
        switch (op1) {
        case 0x00:
            switch (op2) {
            case 0x00:    /
                LOG_DIS("l.sll r%d, r%d, r%d\n", rd, ra, rb);
                tcg_gen_shl_tl(cpu_R[rd], cpu_R[ra], cpu_R[rb]);
                break;
            case 0x01:    /
                LOG_DIS("l.srl r%d, r%d, r%d\n", rd, ra, rb);
                tcg_gen_shr_tl(cpu_R[rd], cpu_R[ra], cpu_R[rb]);
                break;
            case 0x02:    /
                LOG_DIS("l.sra r%d, r%d, r%d\n", rd, ra, rb);
                tcg_gen_sar_tl(cpu_R[rd], cpu_R[ra], cpu_R[rb]);
                break;
            case 0x03:    /
                LOG_DIS("l.ror r%d, r%d, r%d\n", rd, ra, rb);
                tcg_gen_rotr_tl(cpu_R[rd], cpu_R[ra], cpu_R[rb]);
                break;

            default:
                gen_illegal_exception(dc);
                break;
            }
            break;

        default:
            gen_illegal_exception(dc);
            break;
        }
        break;

    case 0x000c:
        switch (op1) {
        case 0x00:
            switch (op2) {
            case 0x00:    /
                LOG_DIS("l.exths r%d, r%d\n", rd, ra);
                tcg_gen_ext16s_tl(cpu_R[rd], cpu_R[ra]);
                break;
            case 0x01:    /
                LOG_DIS("l.extbs r%d, r%d\n", rd, ra);
                tcg_gen_ext8s_tl(cpu_R[rd], cpu_R[ra]);
                break;
            case 0x02:    /
                LOG_DIS("l.exthz r%d, r%d\n", rd, ra);
                tcg_gen_ext16u_tl(cpu_R[rd], cpu_R[ra]);
                break;
            case 0x03:    /
                LOG_DIS("l.extbz r%d, r%d\n", rd, ra);
                tcg_gen_ext8u_tl(cpu_R[rd], cpu_R[ra]);
                break;

            default:
                gen_illegal_exception(dc);
                break;
            }
            break;

        default:
            gen_illegal_exception(dc);
            break;
        }
        break;

    case 0x000d:
        switch (op1) {
        case 0x00:
            switch (op2) {
            case 0x00:    /
                LOG_DIS("l.extws r%d, r%d\n", rd, ra);
                tcg_gen_ext32s_tl(cpu_R[rd], cpu_R[ra]);
                break;
            case 0x01:    /
                LOG_DIS("l.extwz r%d, r%d\n", rd, ra);
                tcg_gen_ext32u_tl(cpu_R[rd], cpu_R[ra]);
                break;

            default:
                gen_illegal_exception(dc);
                break;
            }
            break;

        default:
            gen_illegal_exception(dc);
            break;
        }
        break;

    default:
        gen_illegal_exception(dc);
        break;
    }
}
