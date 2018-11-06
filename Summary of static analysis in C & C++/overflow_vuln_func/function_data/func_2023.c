static void decode_rr_divide(CPUTriCoreState *env, DisasContext *ctx)
{
    uint32_t op2;
    int r1, r2, r3;

    TCGv temp, temp2;

    op2 = MASK_OP_RR_OP2(ctx->opcode);
    r3 = MASK_OP_RR_D(ctx->opcode);
    r2 = MASK_OP_RR_S2(ctx->opcode);
    r1 = MASK_OP_RR_S1(ctx->opcode);

    switch (op2) {
    case OPC2_32_RR_BMERGE:
        gen_helper_bmerge(cpu_gpr_d[r3], cpu_gpr_d[r1], cpu_gpr_d[r2]);
        break;
    case OPC2_32_RR_BSPLIT:
        gen_bsplit(cpu_gpr_d[r3], cpu_gpr_d[r3+1], cpu_gpr_d[r1]);
        break;
    case OPC2_32_RR_DVINIT_B:
        gen_dvinit_b(env, cpu_gpr_d[r3], cpu_gpr_d[r3+1], cpu_gpr_d[r1],
                     cpu_gpr_d[r2]);
        break;
    case OPC2_32_RR_DVINIT_BU:
        temp = tcg_temp_new();
        temp2 = tcg_temp_new();
        /
        tcg_gen_movi_tl(cpu_PSW_AV, 0);
        if (!tricore_feature(env, TRICORE_FEATURE_131)) {
            /
            tcg_gen_neg_tl(temp, cpu_gpr_d[r3+1]);
            /
            tcg_gen_movcond_tl(TCG_COND_LT, temp, cpu_gpr_d[r3+1], cpu_PSW_AV,
                               temp, cpu_gpr_d[r3+1]);
            tcg_gen_neg_tl(temp2, cpu_gpr_d[r2]);
            tcg_gen_movcond_tl(TCG_COND_LT, temp2, cpu_gpr_d[r2], cpu_PSW_AV,
                               temp2, cpu_gpr_d[r2]);
            tcg_gen_setcond_tl(TCG_COND_GE, cpu_PSW_V, temp, temp2);
        } else {
            /
            tcg_gen_setcondi_tl(TCG_COND_EQ, cpu_PSW_V, cpu_gpr_d[r2], 0);
        }
        tcg_gen_shli_tl(cpu_PSW_V, cpu_PSW_V, 31);
        /
        tcg_gen_or_tl(cpu_PSW_SV, cpu_PSW_SV, cpu_PSW_V);
        /
        tcg_gen_shri_tl(temp, cpu_gpr_d[r1], 8);
        tcg_gen_shli_tl(cpu_gpr_d[r3], cpu_gpr_d[r1], 24);
        tcg_gen_mov_tl(cpu_gpr_d[r3+1], temp);

        tcg_temp_free(temp);
        tcg_temp_free(temp2);
        break;
    case OPC2_32_RR_DVINIT_H:
        gen_dvinit_h(env, cpu_gpr_d[r3], cpu_gpr_d[r3+1], cpu_gpr_d[r1],
                     cpu_gpr_d[r2]);
        break;
    case OPC2_32_RR_DVINIT_HU:
        temp = tcg_temp_new();
        temp2 = tcg_temp_new();
        /
        tcg_gen_movi_tl(cpu_PSW_AV, 0);
        if (!tricore_feature(env, TRICORE_FEATURE_131)) {
            /
            tcg_gen_neg_tl(temp, cpu_gpr_d[r3+1]);
            /
            tcg_gen_movcond_tl(TCG_COND_LT, temp, cpu_gpr_d[r3+1], cpu_PSW_AV,
                               temp, cpu_gpr_d[r3+1]);
            tcg_gen_neg_tl(temp2, cpu_gpr_d[r2]);
            tcg_gen_movcond_tl(TCG_COND_LT, temp2, cpu_gpr_d[r2], cpu_PSW_AV,
                               temp2, cpu_gpr_d[r2]);
            tcg_gen_setcond_tl(TCG_COND_GE, cpu_PSW_V, temp, temp2);
        } else {
            /
            tcg_gen_setcondi_tl(TCG_COND_EQ, cpu_PSW_V, cpu_gpr_d[r2], 0);
        }
        tcg_gen_shli_tl(cpu_PSW_V, cpu_PSW_V, 31);
        /
        tcg_gen_or_tl(cpu_PSW_SV, cpu_PSW_SV, cpu_PSW_V);
        /
        tcg_gen_mov_tl(temp, cpu_gpr_d[r1]);
        tcg_gen_shri_tl(cpu_gpr_d[r3+1], temp, 16);
        tcg_gen_shli_tl(cpu_gpr_d[r3], temp, 16);
        tcg_temp_free(temp);
        tcg_temp_free(temp2);
        break;
    case OPC2_32_RR_DVINIT:
        temp = tcg_temp_new();
        temp2 = tcg_temp_new();
        /
        tcg_gen_setcondi_tl(TCG_COND_EQ, temp, cpu_gpr_d[r2], 0xffffffff);
        tcg_gen_setcondi_tl(TCG_COND_EQ, temp2, cpu_gpr_d[r1], 0x80000000);
        tcg_gen_and_tl(temp, temp, temp2);
        tcg_gen_setcondi_tl(TCG_COND_EQ, temp2, cpu_gpr_d[r2], 0);
        tcg_gen_or_tl(cpu_PSW_V, temp, temp2);
        tcg_gen_shli_tl(cpu_PSW_V, cpu_PSW_V, 31);
        /
        tcg_gen_or_tl(cpu_PSW_SV, cpu_PSW_SV, cpu_PSW_V);
        /
       tcg_gen_movi_tl(cpu_PSW_AV, 0);
        /
        tcg_gen_mov_tl(cpu_gpr_d[r3], cpu_gpr_d[r1]);
        /
        tcg_gen_sari_tl(cpu_gpr_d[r3+1], cpu_gpr_d[r1], 31);
        tcg_temp_free(temp);
        tcg_temp_free(temp2);
        break;
    case OPC2_32_RR_DVINIT_U:
        /
        tcg_gen_setcondi_tl(TCG_COND_EQ, cpu_PSW_V, cpu_gpr_d[r2], 0);
        tcg_gen_shli_tl(cpu_PSW_V, cpu_PSW_V, 31);
        /
        tcg_gen_or_tl(cpu_PSW_SV, cpu_PSW_SV, cpu_PSW_V);
        /
        tcg_gen_movi_tl(cpu_PSW_AV, 0);
        /
        tcg_gen_mov_tl(cpu_gpr_d[r3], cpu_gpr_d[r1]);
        /
        tcg_gen_movi_tl(cpu_gpr_d[r3+1], 0);
        break;
    case OPC2_32_RR_PARITY:
        gen_helper_parity(cpu_gpr_d[r3], cpu_gpr_d[r1]);
        break;
    case OPC2_32_RR_UNPACK:
        gen_unpack(cpu_gpr_d[r3], cpu_gpr_d[r3+1], cpu_gpr_d[r1]);
        break;
    }
}
