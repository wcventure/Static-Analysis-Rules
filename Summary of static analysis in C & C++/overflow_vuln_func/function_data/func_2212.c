static void gen_wsr_windowstart(DisasContext *dc, uint32_t sr, TCGv_i32 v)
{
    tcg_gen_andi_i32(cpu_SR[sr], v, (1 << dc->config->nareg / 4) - 1);
    reset_used_window(dc);
}
