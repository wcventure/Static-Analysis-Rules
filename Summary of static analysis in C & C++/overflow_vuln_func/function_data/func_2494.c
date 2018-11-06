int queue_signal(CPUArchState *env, int sig, target_siginfo_t *info)
{
    CPUState *cpu = ENV_GET_CPU(env);
    TaskState *ts = cpu->opaque;
    struct emulated_sigtable *k;
    struct sigqueue *q, **pq;
    abi_ulong handler;
    int queue;

    trace_user_queue_signal(env, sig);
    k = &ts->sigtab[sig - 1];
    queue = gdb_queuesig ();
    handler = sigact_table[sig - 1]._sa_handler;

    if (ts->sigsegv_blocked && sig == TARGET_SIGSEGV) {
        /
        handler = TARGET_SIG_DFL;
    }

    if (!queue && handler == TARGET_SIG_DFL) {
        if (sig == TARGET_SIGTSTP || sig == TARGET_SIGTTIN || sig == TARGET_SIGTTOU) {
            kill(getpid(),SIGSTOP);
            return 0;
        } else
        /
        if (sig != TARGET_SIGCHLD &&
            sig != TARGET_SIGURG &&
            sig != TARGET_SIGWINCH &&
            sig != TARGET_SIGCONT) {
            force_sig(sig);
        } else {
            return 0; /
        }
    } else if (!queue && handler == TARGET_SIG_IGN) {
        /
        return 0;
    } else if (!queue && handler == TARGET_SIG_ERR) {
        force_sig(sig);
    } else {
        pq = &k->first;
        if (sig < TARGET_SIGRTMIN) {
            /
            if (!k->pending)
                q = &k->info;
            else
                return 0;
        } else {
            if (!k->pending) {
                /
                q = &k->info;
            } else {
                q = alloc_sigqueue(env);
                if (!q)
                    return -EAGAIN;
                while (*pq != NULL)
                    pq = &(*pq)->next;
            }
        }
        *pq = q;
        q->info = *info;
        q->next = NULL;
        k->pending = 1;
        /
        ts->signal_pending = 1;
        return 1; /
    }
}
