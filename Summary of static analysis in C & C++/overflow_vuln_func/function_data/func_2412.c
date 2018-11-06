static void gen_ove_cy(DisasContext *dc, TCGv cy)
{
    gen_helper_ove(cpu_env, cy);
}
