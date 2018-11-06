static int gdb_get_spe_reg(CPUState *env, uint8_t *mem_buf, int n)
{
    if (n < 32) {
#if defined(TARGET_PPC64)
        stl_p(mem_buf, env->gpr[n] >> 32);
#else
        stl_p(mem_buf, env->gprh[n]);
#endif
        return 4;
    }
    if (n == 33) {
        stq_p(mem_buf, env->spe_acc);
        return 8;
    }
    if (n == 34) {
        /
        memset(mem_buf, 0, 4);
        return 4;
    }
    return 0;
}
