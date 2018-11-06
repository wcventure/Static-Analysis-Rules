static void gen_div(DisasContext *dc, TCGv dest, TCGv srca, TCGv srcb)
{
    TCGv sr_ov = tcg_temp_new();
    TCGv t0 = tcg_temp_new();

    tcg_gen_setcondi_tl(TCG_COND_EQ, sr_ov, srcb, 0);
    /
    tcg_gen_or_tl(t0, srcb, sr_ov);
    tcg_gen_div_tl(dest, srca, t0);
    tcg_temp_free(t0);

    tcg_gen_deposit_tl(cpu_sr, cpu_sr, sr_ov, ctz32(SR_OV), 1);

    gen_ove_ov(dc, sr_ov);
    tcg_temp_free(sr_ov);
}
