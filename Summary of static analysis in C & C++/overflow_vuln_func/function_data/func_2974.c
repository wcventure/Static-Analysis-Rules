static inline void gen_op_fpexception_im(int fsr_flags)
{
    TCGv r_const;

    tcg_gen_andi_tl(cpu_fsr, cpu_fsr, ~FSR_FTT_MASK);
    tcg_gen_ori_tl(cpu_fsr, cpu_fsr, fsr_flags);
    r_const = tcg_const_i32(TT_FP_EXCP);
    tcg_gen_helper_0_1(raise_exception, r_const);
    tcg_temp_free(r_const);
}
