void process_pending_signals(CPUArchState *cpu_env)
{
    CPUState *cpu = ENV_GET_CPU(cpu_env);
    int sig;
    TaskState *ts = cpu->opaque;

    if (!ts->signal_pending)
        return;

    /
    for(sig = 1; sig <= TARGET_NSIG; sig++) {
        if (ts->sigtab[sig - 1].pending) {
            handle_pending_signal(cpu_env, sig);
            return;
        }
    }
    /
    ts->signal_pending = 0;
    return;
}
