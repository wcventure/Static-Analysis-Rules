static void gen_sub(DisasContext *dc, TCGv dest, TCGv srca, TCGv srcb)
{
    TCGv res = tcg_temp_new();
    TCGv sr_cy = tcg_temp_new();
    TCGv sr_ov = tcg_temp_new();

    tcg_gen_sub_tl(res, srca, srcb);
    tcg_gen_xor_tl(sr_cy, srca, srcb);
    tcg_gen_xor_tl(sr_ov, res, srcb);
    tcg_gen_and_tl(sr_ov, sr_ov, sr_cy);
    tcg_gen_setcond_tl(TCG_COND_LTU, sr_cy, srca, srcb);

    tcg_gen_mov_tl(dest, res);
    tcg_temp_free(res);

    tcg_gen_shri_tl(sr_ov, sr_ov, TARGET_LONG_BITS - 1);
    tcg_gen_deposit_tl(cpu_sr, cpu_sr, sr_cy, ctz32(SR_CY), 1);
    tcg_gen_deposit_tl(cpu_sr, cpu_sr, sr_ov, ctz32(SR_OV), 1);

    gen_ove_cyov(dc, sr_ov, sr_cy);
    tcg_temp_free(sr_ov);
    tcg_temp_free(sr_cy);
}
