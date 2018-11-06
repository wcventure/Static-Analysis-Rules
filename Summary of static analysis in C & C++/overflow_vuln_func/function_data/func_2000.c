static void gen_ove_ov(DisasContext *dc, TCGv ov)
{
    if (dc->tb_flags & SR_OVE) {
        gen_helper_ove(cpu_env, ov);
    }
}
