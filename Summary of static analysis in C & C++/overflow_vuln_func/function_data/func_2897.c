void cpu_interrupt(CPUState *env, int mask)
{
#if !defined(USE_NPTL)
    TranslationBlock *tb;
    static spinlock_t interrupt_lock = SPIN_LOCK_UNLOCKED;
#endif
    int old_mask;

    old_mask = env->interrupt_request;
    /
    env->interrupt_request |= mask;
#if defined(USE_NPTL)
    /
#else
    if (use_icount) {
        env->icount_decr.u16.high = 0x8000;
#ifndef CONFIG_USER_ONLY
        /
        if (!can_do_io(env)
            && (mask & ~(old_mask | CPU_INTERRUPT_EXIT)) != 0) {
            cpu_abort(env, "Raised interrupt while not in I/O function");
        }
#endif
    } else {
        tb = env->current_tb;
        /
        if (tb && !testandset(&interrupt_lock)) {
            env->current_tb = NULL;
            tb_reset_jump_recursive(tb);
            resetlock(&interrupt_lock);
        }
    }
#endif
}
