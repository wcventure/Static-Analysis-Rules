static void gen_mulu(DisasContext *dc, TCGv dest, TCGv srca, TCGv srcb)
{
    TCGv sr_cy = tcg_temp_new();

    tcg_gen_muls2_tl(dest, sr_cy, srca, srcb);
    tcg_gen_setcondi_tl(TCG_COND_NE, sr_cy, sr_cy, 0);

    tcg_gen_deposit_tl(cpu_sr, cpu_sr, sr_cy, ctz32(SR_CY), 1);

    gen_ove_cy(dc, sr_cy);
    tcg_temp_free(sr_cy);
}
