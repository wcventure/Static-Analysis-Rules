Coroutine *qemu_coroutine_new(void)
{
    const size_t stack_size = 1 << 20;
    CoroutineUContext *co;
    CoroutineThreadState *coTS;
    struct sigaction sa;
    struct sigaction osa;
    stack_t ss;
    stack_t oss;
    sigset_t sigs;
    sigset_t osigs;
    jmp_buf old_env;

    /

    co = g_malloc0(sizeof(*co));
    co->stack = g_malloc(stack_size);
    co->base.entry_arg = &old_env; /

    coTS = coroutine_get_thread_state();
    coTS->tr_handler = co;

    /
    sigemptyset(&sigs);
    sigaddset(&sigs, SIGUSR2);
    pthread_sigmask(SIG_BLOCK, &sigs, &osigs);
    sa.sa_handler = coroutine_trampoline;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = SA_ONSTACK;
    if (sigaction(SIGUSR2, &sa, &osa) != 0) {
        abort();
    }

    /
    ss.ss_sp = co->stack;
    ss.ss_size = stack_size;
    ss.ss_flags = 0;
    if (sigaltstack(&ss, &oss) < 0) {
        abort();
    }

    /
    coTS->tr_called = 0;
    pthread_kill(pthread_self(), SIGUSR2);
    sigfillset(&sigs);
    sigdelset(&sigs, SIGUSR2);
    while (!coTS->tr_called) {
        sigsuspend(&sigs);
    }

    /
    sigaltstack(NULL, &ss);
    ss.ss_flags = SS_DISABLE;
    if (sigaltstack(&ss, NULL) < 0) {
        abort();
    }
    sigaltstack(NULL, &ss);
    if (!(oss.ss_flags & SS_DISABLE)) {
        sigaltstack(&oss, NULL);
    }

    /
    sigaction(SIGUSR2, &osa, NULL);
    pthread_sigmask(SIG_SETMASK, &osigs, NULL);

    /
    if (!sigsetjmp(old_env, 0)) {
        siglongjmp(coTS->tr_reenter, 1);
    }

    /

    return &co->base;
}
