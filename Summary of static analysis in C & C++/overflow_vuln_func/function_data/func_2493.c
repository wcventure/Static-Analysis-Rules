static void set_sigmask(const sigset_t *set)
{
    do_sigprocmask(SIG_SETMASK, set, NULL);
}
