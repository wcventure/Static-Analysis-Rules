static void gen_mul(DisasContext *dc, TCGv dest, TCGv srca, TCGv srcb)
{
    TCGv sr_ov = tcg_temp_new();
    TCGv t0 = tcg_temp_new();

    tcg_gen_muls2_tl(dest, sr_ov, srca, srcb);
    tcg_gen_sari_tl(t0, dest, TARGET_LONG_BITS - 1);
    tcg_gen_setcond_tl(TCG_COND_NE, sr_ov, sr_ov, t0);
    tcg_temp_free(t0);

    tcg_gen_deposit_tl(cpu_sr, cpu_sr, sr_ov, ctz32(SR_OV), 1);

    gen_ove_ov(dc, sr_ov);
    tcg_temp_free(sr_ov);
}
