static void gen_ove_cyov(DisasContext *dc, TCGv cy, TCGv ov)
{
    if (dc->tb_flags & SR_OVE) {
        TCGv t0 = tcg_temp_new();
        tcg_gen_or_tl(t0, cy, ov);
        gen_helper_ove(cpu_env, t0);
        tcg_temp_free(t0);
    }
}
