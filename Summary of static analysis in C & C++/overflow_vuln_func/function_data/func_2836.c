static target_ulong helper_sdiv_common(CPUSPARCState *env, target_ulong a,
                                       target_ulong b, int cc)
{
    SPARCCPU *cpu = sparc_env_get_cpu(env);
    int overflow = 0;
    int64_t x0;
    int32_t x1;

    x0 = (a & 0xffffffff) | ((int64_t) (env->y) << 32);
    x1 = (b & 0xffffffff);

    if (x1 == 0) {
        cpu_restore_state(CPU(cpu), GETPC());
        helper_raise_exception(env, TT_DIV_ZERO);
    }

    x0 = x0 / x1;
    if ((int32_t) x0 != x0) {
        x0 = x0 < 0 ? 0x80000000 : 0x7fffffff;
        overflow = 1;
    }

    if (cc) {
        env->cc_dst = x0;
        env->cc_src2 = overflow;
        env->cc_op = CC_OP_DIV;
    }
    return x0;
}
