static void gen_addc(DisasContext *dc, TCGv dest, TCGv srca, TCGv srcb)
{
    TCGv t0 = tcg_const_tl(0);
    TCGv res = tcg_temp_new();
    TCGv sr_cy = tcg_temp_new();
    TCGv sr_ov = tcg_temp_new();

    tcg_gen_shri_tl(sr_cy, cpu_sr, ctz32(SR_CY));
    tcg_gen_andi_tl(sr_cy, sr_cy, 1);

    tcg_gen_add2_tl(res, sr_cy, srca, t0, sr_cy, t0);
    tcg_gen_add2_tl(res, sr_cy, res, sr_cy, srcb, t0);
    tcg_gen_xor_tl(sr_ov, srca, srcb);
    tcg_gen_xor_tl(t0, res, srcb);
    tcg_gen_andc_tl(sr_ov, t0, sr_ov);
    tcg_temp_free(t0);

    tcg_gen_mov_tl(dest, res);
    tcg_temp_free(res);

    tcg_gen_shri_tl(sr_ov, sr_ov, TARGET_LONG_BITS - 1);
    tcg_gen_deposit_tl(cpu_sr, cpu_sr, sr_cy, ctz32(SR_CY), 1);
    tcg_gen_deposit_tl(cpu_sr, cpu_sr, sr_ov, ctz32(SR_OV), 1);

    gen_ove_cyov(dc, sr_ov, sr_cy);
    tcg_temp_free(sr_ov);
    tcg_temp_free(sr_cy);
}
