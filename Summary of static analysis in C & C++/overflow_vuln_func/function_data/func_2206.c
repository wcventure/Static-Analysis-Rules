void HELPER(window_check)(CPUXtensaState *env, uint32_t pc, uint32_t w)
{
    uint32_t windowbase = windowbase_bound(env->sregs[WINDOW_BASE], env);
    uint32_t windowstart = env->sregs[WINDOW_START];
    uint32_t m, n;

    if ((env->sregs[PS] & (PS_WOE | PS_EXCM)) ^ PS_WOE) {
        return;
    }

    for (n = 1; ; ++n) {
        if (n > w) {
            return;
        }
        if (windowstart & windowstart_bit(windowbase + n, env)) {
            break;
        }
    }

    m = windowbase_bound(windowbase + n, env);
    rotate_window(env, n);
    env->sregs[PS] = (env->sregs[PS] & ~PS_OWB) |
        (windowbase << PS_OWB_SHIFT) | PS_EXCM;
    env->sregs[EPC1] = env->pc = pc;

    if (windowstart & windowstart_bit(m + 1, env)) {
        HELPER(exception)(env, EXC_WINDOW_OVERFLOW4);
    } else if (windowstart & windowstart_bit(m + 2, env)) {
        HELPER(exception)(env, EXC_WINDOW_OVERFLOW8);
    } else {
        HELPER(exception)(env, EXC_WINDOW_OVERFLOW12);
    }
}
