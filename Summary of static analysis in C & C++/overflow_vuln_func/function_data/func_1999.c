static void gen_ove_cy(DisasContext *dc, TCGv cy)
{
    if (dc->tb_flags & SR_OVE) {
        gen_helper_ove(cpu_env, cy);
    }
}
