int do_sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
    int ret;
    sigset_t val;
    sigset_t *temp = NULL;
    CPUState *cpu = thread_cpu;
    TaskState *ts = (TaskState *)cpu->opaque;
    bool segv_was_blocked = ts->sigsegv_blocked;

    if (set) {
        bool has_sigsegv = sigismember(set, SIGSEGV);
        val = *set;
        temp = &val;

        sigdelset(temp, SIGSEGV);

        switch (how) {
        case SIG_BLOCK:
            if (has_sigsegv) {
                ts->sigsegv_blocked = true;
            }
            break;
        case SIG_UNBLOCK:
            if (has_sigsegv) {
                ts->sigsegv_blocked = false;
            }
            break;
        case SIG_SETMASK:
            ts->sigsegv_blocked = has_sigsegv;
            break;
        default:
            g_assert_not_reached();
        }
    }

    ret = sigprocmask(how, temp, oldset);

    if (oldset && segv_was_blocked) {
        sigaddset(oldset, SIGSEGV);
    }

    return ret;
}
