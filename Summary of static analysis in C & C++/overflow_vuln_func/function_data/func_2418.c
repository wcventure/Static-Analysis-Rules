static void dec_misc(DisasContext *dc, uint32_t insn)
{
    uint32_t op0, op1;
    uint32_t ra, rb, rd;
    uint32_t L6, K5, K16, K5_11;
    int32_t I16, I5_11, N26;
    TCGMemOp mop;

    op0 = extract32(insn, 26, 6);
    op1 = extract32(insn, 24, 2);
    ra = extract32(insn, 16, 5);
    rb = extract32(insn, 11, 5);
    rd = extract32(insn, 21, 5);
    L6 = extract32(insn, 5, 6);
    K5 = extract32(insn, 0, 5);
    K16 = extract32(insn, 0, 16);
    I16 = (int16_t)K16;
    N26 = sextract32(insn, 0, 26);
    K5_11 = (extract32(insn, 21, 5) << 11) | extract32(insn, 0, 11);
    I5_11 = (int16_t)K5_11;

    switch (op0) {
    case 0x00:    /
        LOG_DIS("l.j %d\n", N26);
        gen_jump(dc, N26, 0, op0);
        break;

    case 0x01:    /
        LOG_DIS("l.jal %d\n", N26);
        gen_jump(dc, N26, 0, op0);
        break;

    case 0x03:    /
        LOG_DIS("l.bnf %d\n", N26);
        gen_jump(dc, N26, 0, op0);
        break;

    case 0x04:    /
        LOG_DIS("l.bf %d\n", N26);
        gen_jump(dc, N26, 0, op0);
        break;

    case 0x05:
        switch (op1) {
        case 0x01:    /
            LOG_DIS("l.nop %d\n", I16);
            break;

        default:
            gen_illegal_exception(dc);
            break;
        }
        break;

    case 0x11:    /
        LOG_DIS("l.jr r%d\n", rb);
         gen_jump(dc, 0, rb, op0);
         break;

    case 0x12:    /
        LOG_DIS("l.jalr r%d\n", rb);
        gen_jump(dc, 0, rb, op0);
        break;

    case 0x13:    /
        LOG_DIS("l.maci r%d, %d\n", ra, I16);
        {
            TCGv_i64 t1 = tcg_temp_new_i64();
            TCGv_i64 t2 = tcg_temp_new_i64();
            TCGv_i32 dst = tcg_temp_new_i32();
            TCGv ttmp = tcg_const_tl(I16);
            tcg_gen_mul_tl(dst, cpu_R[ra], ttmp);
            tcg_gen_ext_i32_i64(t1, dst);
            tcg_gen_concat_i32_i64(t2, maclo, machi);
            tcg_gen_add_i64(t2, t2, t1);
            tcg_gen_extrl_i64_i32(maclo, t2);
            tcg_gen_shri_i64(t2, t2, 32);
            tcg_gen_extrl_i64_i32(machi, t2);
            tcg_temp_free_i32(dst);
            tcg_temp_free(ttmp);
            tcg_temp_free_i64(t1);
            tcg_temp_free_i64(t2);
        }
        break;

    case 0x09:    /
        LOG_DIS("l.rfe\n");
        {
#if defined(CONFIG_USER_ONLY)
            return;
#else
            if (dc->mem_idx == MMU_USER_IDX) {
                gen_illegal_exception(dc);
                return;
            }
            gen_helper_rfe(cpu_env);
            dc->is_jmp = DISAS_UPDATE;
#endif
        }
        break;

    case 0x1b: /
        LOG_DIS("l.lwa r%d, r%d, %d\n", rd, ra, I16);
        gen_lwa(dc, cpu_R[rd], cpu_R[ra], I16);
        break;

    case 0x1c:    /
        LOG_DIS("l.cust1\n");
        break;

    case 0x1d:    /
        LOG_DIS("l.cust2\n");
        break;

    case 0x1e:    /
        LOG_DIS("l.cust3\n");
        break;

    case 0x1f:    /
        LOG_DIS("l.cust4\n");
        break;

    case 0x3c:    /
        LOG_DIS("l.cust5 r%d, r%d, r%d, %d, %d\n", rd, ra, rb, L6, K5);
        break;

    case 0x3d:    /
        LOG_DIS("l.cust6\n");
        break;

    case 0x3e:    /
        LOG_DIS("l.cust7\n");
        break;

    case 0x3f:    /
        LOG_DIS("l.cust8\n");
        break;

/
/

    case 0x21:    /
        LOG_DIS("l.lwz r%d, r%d, %d\n", rd, ra, I16);
        mop = MO_TEUL;
        goto do_load;

    case 0x22:    /
        LOG_DIS("l.lws r%d, r%d, %d\n", rd, ra, I16);
        mop = MO_TESL;
        goto do_load;

    case 0x23:    /
        LOG_DIS("l.lbz r%d, r%d, %d\n", rd, ra, I16);
        mop = MO_UB;
        goto do_load;

    case 0x24:    /
        LOG_DIS("l.lbs r%d, r%d, %d\n", rd, ra, I16);
        mop = MO_SB;
        goto do_load;

    case 0x25:    /
        LOG_DIS("l.lhz r%d, r%d, %d\n", rd, ra, I16);
        mop = MO_TEUW;
        goto do_load;

    case 0x26:    /
        LOG_DIS("l.lhs r%d, r%d, %d\n", rd, ra, I16);
        mop = MO_TESW;
        goto do_load;

    do_load:
        {
            TCGv t0 = tcg_temp_new();
            tcg_gen_addi_tl(t0, cpu_R[ra], I16);
            tcg_gen_qemu_ld_tl(cpu_R[rd], t0, dc->mem_idx, mop);
            tcg_temp_free(t0);
        }
        break;

    case 0x27:    /
        LOG_DIS("l.addi r%d, r%d, %d\n", rd, ra, I16);
        {
            if (I16 == 0) {
                tcg_gen_mov_tl(cpu_R[rd], cpu_R[ra]);
            } else {
                TCGLabel *lab = gen_new_label();
                TCGv_i64 ta = tcg_temp_new_i64();
                TCGv_i64 td = tcg_temp_local_new_i64();
                TCGv_i32 res = tcg_temp_local_new_i32();
                TCGv_i32 sr_ove = tcg_temp_local_new_i32();
                tcg_gen_extu_i32_i64(ta, cpu_R[ra]);
                tcg_gen_addi_i64(td, ta, I16);
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
                tcg_temp_free_i64(td);
                tcg_temp_free_i32(res);
                tcg_temp_free_i32(sr_ove);
            }
        }
        break;

    case 0x28:    /
        LOG_DIS("l.addic r%d, r%d, %d\n", rd, ra, I16);
        {
            TCGLabel *lab = gen_new_label();
            TCGv_i64 ta = tcg_temp_new_i64();
            TCGv_i64 td = tcg_temp_local_new_i64();
            TCGv_i64 tcy = tcg_temp_local_new_i64();
            TCGv_i32 res = tcg_temp_local_new_i32();
            TCGv_i32 sr_cy = tcg_temp_local_new_i32();
            TCGv_i32 sr_ove = tcg_temp_local_new_i32();
            tcg_gen_extu_i32_i64(ta, cpu_R[ra]);
            tcg_gen_andi_i32(sr_cy, cpu_sr, SR_CY);
            tcg_gen_shri_i32(sr_cy, sr_cy, 10);
            tcg_gen_extu_i32_i64(tcy, sr_cy);
            tcg_gen_addi_i64(td, ta, I16);
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
            tcg_temp_free_i64(td);
            tcg_temp_free_i64(tcy);
            tcg_temp_free_i32(res);
            tcg_temp_free_i32(sr_cy);
            tcg_temp_free_i32(sr_ove);
        }
        break;

    case 0x29:    /
        LOG_DIS("l.andi r%d, r%d, %d\n", rd, ra, K16);
        tcg_gen_andi_tl(cpu_R[rd], cpu_R[ra], K16);
        break;

    case 0x2a:    /
        LOG_DIS("l.ori r%d, r%d, %d\n", rd, ra, K16);
        tcg_gen_ori_tl(cpu_R[rd], cpu_R[ra], K16);
        break;

    case 0x2b:    /
        LOG_DIS("l.xori r%d, r%d, %d\n", rd, ra, I16);
        tcg_gen_xori_tl(cpu_R[rd], cpu_R[ra], I16);
        break;

    case 0x2c:    /
        LOG_DIS("l.muli r%d, r%d, %d\n", rd, ra, I16);
        if (ra != 0 && I16 != 0) {
            TCGv_i32 im = tcg_const_i32(I16);
            gen_helper_mul32(cpu_R[rd], cpu_env, cpu_R[ra], im);
            tcg_temp_free_i32(im);
        } else {
            tcg_gen_movi_tl(cpu_R[rd], 0x0);
        }
        break;

    case 0x2d:    /
        LOG_DIS("l.mfspr r%d, r%d, %d\n", rd, ra, K16);
        {
#if defined(CONFIG_USER_ONLY)
            return;
#else
            TCGv_i32 ti = tcg_const_i32(K16);
            if (dc->mem_idx == MMU_USER_IDX) {
                gen_illegal_exception(dc);
                return;
            }
            gen_helper_mfspr(cpu_R[rd], cpu_env, cpu_R[rd], cpu_R[ra], ti);
            tcg_temp_free_i32(ti);
#endif
        }
        break;

    case 0x30:    /
        LOG_DIS("l.mtspr r%d, r%d, %d\n", ra, rb, K5_11);
        {
#if defined(CONFIG_USER_ONLY)
            return;
#else
            TCGv_i32 im = tcg_const_i32(K5_11);
            if (dc->mem_idx == MMU_USER_IDX) {
                gen_illegal_exception(dc);
                return;
            }
            gen_helper_mtspr(cpu_env, cpu_R[ra], cpu_R[rb], im);
            tcg_temp_free_i32(im);
#endif
        }
        break;

    case 0x33: /
        LOG_DIS("l.swa r%d, r%d, %d\n", ra, rb, I5_11);
        gen_swa(dc, cpu_R[rb], cpu_R[ra], I5_11);
        break;

/
/

    case 0x35:    /
        LOG_DIS("l.sw r%d, r%d, %d\n", ra, rb, I5_11);
        mop = MO_TEUL;
        goto do_store;

    case 0x36:    /
        LOG_DIS("l.sb r%d, r%d, %d\n", ra, rb, I5_11);
        mop = MO_UB;
        goto do_store;

    case 0x37:    /
        LOG_DIS("l.sh r%d, r%d, %d\n", ra, rb, I5_11);
        mop = MO_TEUW;
        goto do_store;

    do_store:
        {
            TCGv t0 = tcg_temp_new();
            tcg_gen_addi_tl(t0, cpu_R[ra], I5_11);
            tcg_gen_qemu_st_tl(cpu_R[rb], t0, dc->mem_idx, mop);
            tcg_temp_free(t0);
        }
        break;

    default:
        gen_illegal_exception(dc);
        break;
    }
}
