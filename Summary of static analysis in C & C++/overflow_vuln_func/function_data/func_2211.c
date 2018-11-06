static void gen_wsr_windowbase(DisasContext *dc, uint32_t sr, TCGv_i32 v)
{
    gen_helper_wsr_windowbase(cpu_env, v);
    reset_used_window(dc);
}
