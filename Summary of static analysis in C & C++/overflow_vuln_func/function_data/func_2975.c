static inline void gen_op_clear_ieee_excp_and_FTT(void)
{
    tcg_gen_andi_tl(cpu_fsr, cpu_fsr, ~(FSR_FTT_MASK | FSR_CEXC_MASK));
}
