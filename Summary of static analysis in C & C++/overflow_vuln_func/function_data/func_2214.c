static void gen_window_check1(DisasContext *dc, unsigned r1)
{
    if (dc->tb->flags & XTENSA_TBFLAG_EXCM) {
        return;
    }
    if (option_enabled(dc, XTENSA_OPTION_WINDOWED_REGISTER) &&
            r1 / 4 > dc->used_window) {
        int label = gen_new_label();
        TCGv_i32 ws = tcg_temp_new_i32();

        dc->used_window = r1 / 4;
        tcg_gen_deposit_i32(ws, cpu_SR[WINDOW_START], cpu_SR[WINDOW_START],
                dc->config->nareg / 4, dc->config->nareg / 4);
        tcg_gen_shr_i32(ws, ws, cpu_SR[WINDOW_BASE]);
        tcg_gen_andi_i32(ws, ws, (2 << (r1 / 4)) - 2);
        tcg_gen_brcondi_i32(TCG_COND_EQ, ws, 0, label);
        {
            TCGv_i32 pc = tcg_const_i32(dc->pc);
            TCGv_i32 w = tcg_const_i32(r1 / 4);

            gen_advance_ccount_cond(dc);
            gen_helper_window_check(cpu_env, pc, w);

            tcg_temp_free(w);
            tcg_temp_free(pc);
        }
        gen_set_label(label);
        tcg_temp_free(ws);
    }
}
