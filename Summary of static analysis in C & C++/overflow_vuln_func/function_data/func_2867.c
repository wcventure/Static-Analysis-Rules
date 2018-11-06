int mips_cpu_gdb_write_register(CPUState *cs, uint8_t *mem_buf, int n)
{
    MIPSCPU *cpu = MIPS_CPU(cs);
    CPUMIPSState *env = &cpu->env;
    target_ulong tmp;

    tmp = ldtul_p(mem_buf);

    if (n < 32) {
        env->active_tc.gpr[n] = tmp;
        return sizeof(target_ulong);
    }
    if (env->CP0_Config1 & (1 << CP0C1_FP)
            && n >= 38 && n < 73) {
        if (n < 70) {
            if (env->CP0_Status & (1 << CP0St_FR)) {
                env->active_fpu.fpr[n - 38].d = tmp;
            } else {
                env->active_fpu.fpr[n - 38].w[FP_ENDIAN_IDX] = tmp;
            }
        }
        switch (n) {
        case 70:
            env->active_fpu.fcr31 = tmp & 0xFF83FFFF;
            /
            RESTORE_ROUNDING_MODE;
            break;
        case 71:
            env->active_fpu.fcr0 = tmp;
            break;
        }
        return sizeof(target_ulong);
    }
    switch (n) {
    case 32:
        env->CP0_Status = tmp;
        break;
    case 33:
        env->active_tc.LO[0] = tmp;
        break;
    case 34:
        env->active_tc.HI[0] = tmp;
        break;
    case 35:
        env->CP0_BadVAddr = tmp;
        break;
    case 36:
        env->CP0_Cause = tmp;
        break;
    case 37:
        env->active_tc.PC = tmp & ~(target_ulong)1;
        if (tmp & 1) {
            env->hflags |= MIPS_HFLAG_M16;
        } else {
            env->hflags &= ~(MIPS_HFLAG_M16);
        }
        break;
    case 72: /
        break;
    default:
        if (n > 89) {
            return 0;
        }
        /
        break;
    }

    return sizeof(target_ulong);
}
