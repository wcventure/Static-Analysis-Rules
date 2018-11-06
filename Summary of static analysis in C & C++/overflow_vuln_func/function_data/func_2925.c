static inline void
gen_msub32_q(TCGv ret, TCGv arg1, TCGv arg2, TCGv arg3, uint32_t n,
             uint32_t up_shift, CPUTriCoreState *env)
{
    TCGv temp = tcg_temp_new();
    TCGv temp2 = tcg_temp_new();
    TCGv temp3 = tcg_temp_new();
    TCGv_i64 t1 = tcg_temp_new_i64();
    TCGv_i64 t2 = tcg_temp_new_i64();
    TCGv_i64 t3 = tcg_temp_new_i64();
    TCGv_i64 t4 = tcg_temp_new_i64();

    tcg_gen_ext_i32_i64(t2, arg2);
    tcg_gen_ext_i32_i64(t3, arg3);

    tcg_gen_mul_i64(t2, t2, t3);

    tcg_gen_ext_i32_i64(t1, arg1);
    /
    tcg_gen_andi_i64(t4, t2, (1ll << (up_shift - n)) - 1);
    tcg_gen_setcondi_i64(TCG_COND_NE, t4, t4, 0);
    tcg_gen_sari_i64(t2, t2, up_shift - n);
    tcg_gen_add_i64(t2, t2, t4);

    tcg_gen_sub_i64(t3, t1, t2);
    tcg_gen_trunc_i64_i32(temp3, t3);
    /
    tcg_gen_setcondi_i64(TCG_COND_GT, t1, t3, 0x7fffffffLL);
    tcg_gen_setcondi_i64(TCG_COND_LT, t2, t3, -0x80000000LL);
    tcg_gen_or_i64(t1, t1, t2);
    tcg_gen_trunc_i64_i32(cpu_PSW_V, t1);
    tcg_gen_shli_tl(cpu_PSW_V, cpu_PSW_V, 31);
    /
    if (n == 1) {
        tcg_gen_setcondi_tl(TCG_COND_EQ, temp, arg2, 0x80000000);
        tcg_gen_setcond_tl(TCG_COND_EQ, temp2, arg2, arg3);
        tcg_gen_and_tl(temp, temp, temp2);
        tcg_gen_shli_tl(temp, temp, 31);
        /
        tcg_gen_xor_tl(cpu_PSW_V, cpu_PSW_V, temp);
    }
    /
    tcg_gen_or_tl(cpu_PSW_SV, cpu_PSW_SV, cpu_PSW_V);
    /
    tcg_gen_add_tl(cpu_PSW_AV, temp3, temp3);
    tcg_gen_xor_tl(cpu_PSW_AV, temp3, cpu_PSW_AV);
    /
    tcg_gen_or_tl(cpu_PSW_SAV, cpu_PSW_SAV, cpu_PSW_AV);
    /
    tcg_gen_mov_tl(ret, temp3);

    tcg_temp_free(temp);
    tcg_temp_free(temp2);
    tcg_temp_free(temp3);
    tcg_temp_free_i64(t1);
    tcg_temp_free_i64(t2);
    tcg_temp_free_i64(t3);
    tcg_temp_free_i64(t4);
}
