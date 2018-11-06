static void gen_advance_ccount_cond(DisasContext *dc)
{
    if (dc->ccount_delta > 0) {
        TCGv_i32 tmp = tcg_const_i32(dc->ccount_delta);
        gen_helper_advance_ccount(cpu_env, tmp);
        tcg_temp_free(tmp);
    }
}
