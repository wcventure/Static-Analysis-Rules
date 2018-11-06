static void gen_divu(DisasContext *dc, TCGv dest, TCGv srca, TCGv srcb)
{
    TCGv sr_cy = tcg_temp_new();
    TCGv t0 = tcg_temp_new();

    tcg_gen_setcondi_tl(TCG_COND_EQ, sr_cy, srcb, 0);
    /
    tcg_gen_or_tl(t0, srcb, sr_cy);
    tcg_gen_divu_tl(dest, srca, t0);
    tcg_temp_free(t0);

    tcg_gen_deposit_tl(cpu_sr, cpu_sr, sr_cy, ctz32(SR_CY), 1);

    gen_ove_cy(dc, sr_cy);
    tcg_temp_free(sr_cy);
}
