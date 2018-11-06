static void handle_pending_signal(CPUArchState *cpu_env, int sig)
{
    CPUState *cpu = ENV_GET_CPU(cpu_env);
    abi_ulong handler;
    sigset_t set, old_set;
    target_sigset_t target_old_set;
    struct target_sigaction *sa;
    struct sigqueue *q;
    TaskState *ts = cpu->opaque;
    struct emulated_sigtable *k = &ts->sigtab[sig - 1];

    trace_user_handle_signal(cpu_env, sig);
    /
    q = k->first;
    k->first = q->next;
    if (!k->first)
        k->pending = 0;

    sig = gdb_handlesig(cpu, sig);
    if (!sig) {
        sa = NULL;
        handler = TARGET_SIG_IGN;
    } else {
        sa = &sigact_table[sig - 1];
        handler = sa->_sa_handler;
    }

    if (ts->sigsegv_blocked && sig == TARGET_SIGSEGV) {
        /
        handler = TARGET_SIG_DFL;
    }

    if (handler == TARGET_SIG_DFL) {
        /
        if (sig == TARGET_SIGTSTP || sig == TARGET_SIGTTIN || sig == TARGET_SIGTTOU) {
            kill(getpid(),SIGSTOP);
        } else if (sig != TARGET_SIGCHLD &&
                   sig != TARGET_SIGURG &&
                   sig != TARGET_SIGWINCH &&
                   sig != TARGET_SIGCONT) {
            force_sig(sig);
        }
    } else if (handler == TARGET_SIG_IGN) {
        /
    } else if (handler == TARGET_SIG_ERR) {
        force_sig(sig);
    } else {
        /
        target_to_host_sigset(&set, &sa->sa_mask);
        /
        if (!(sa->sa_flags & TARGET_SA_NODEFER))
            sigaddset(&set, target_to_host_signal(sig));

        /
        do_sigprocmask(SIG_BLOCK, &set, &old_set);
        /
        host_to_target_sigset_internal(&target_old_set, &old_set);

        /
#if defined(TARGET_I386) && !defined(TARGET_X86_64)
        {
            CPUX86State *env = cpu_env;
            if (env->eflags & VM_MASK)
                save_v86_state(env);
        }
#endif
        /
#if defined(TARGET_ABI_MIPSN32) || defined(TARGET_ABI_MIPSN64) \
    || defined(TARGET_OPENRISC) || defined(TARGET_TILEGX)
        /
        setup_rt_frame(sig, sa, &q->info, &target_old_set, cpu_env);
#else
        if (sa->sa_flags & TARGET_SA_SIGINFO)
            setup_rt_frame(sig, sa, &q->info, &target_old_set, cpu_env);
        else
            setup_frame(sig, sa, &target_old_set, cpu_env);
#endif
        if (sa->sa_flags & TARGET_SA_RESETHAND) {
            sa->_sa_handler = TARGET_SIG_DFL;
        }
    }
    if (q != &k->info)
        free_sigqueue(cpu_env, q);
}
