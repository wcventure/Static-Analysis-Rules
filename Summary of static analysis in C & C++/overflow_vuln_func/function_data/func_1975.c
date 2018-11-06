static void booke_update_fixed_timer(CPUPPCState         *env,
                                     uint8_t           target_bit,
                                     uint64_t          *next,
                                     struct QEMUTimer *timer)
{
    ppc_tb_t *tb_env = env->tb_env;
    uint64_t lapse;
    uint64_t tb;
    uint64_t period = 1 << (target_bit + 1);
    uint64_t now;

    now = qemu_get_clock_ns(vm_clock);
    tb  = cpu_ppc_get_tb(tb_env, now, tb_env->tb_offset);

    lapse = period - ((tb - (1 << target_bit)) & (period - 1));

    *next = now + muldiv64(lapse, get_ticks_per_sec(), tb_env->tb_freq);

    /

    if (*next == now) {
        (*next)++;
    }

    qemu_mod_timer(timer, *next);
}
