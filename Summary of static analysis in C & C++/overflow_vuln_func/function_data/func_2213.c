static void gen_wsr_ps(DisasContext *dc, uint32_t sr, TCGv_i32 v)
{
    uint32_t mask = PS_WOE | PS_CALLINC | PS_OWB |
        PS_UM | PS_EXCM | PS_INTLEVEL;

    if (option_enabled(dc, XTENSA_OPTION_MMU)) {
        mask |= PS_RING;
    }
    tcg_gen_andi_i32(cpu_SR[sr], v, mask);
    reset_used_window(dc);
    gen_helper_check_interrupts(cpu_env);
    /
    gen_jumpi_check_loop_end(dc, -1);
}
