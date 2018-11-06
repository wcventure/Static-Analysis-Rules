#if defined(TARGET_MIPS64)
void do_ddiv (void)
{
    if (T1 != 0) {
        lldiv_t res = lldiv((int64_t)T0, (int64_t)T1);
        env->LO[0][env->current_tc] = res.quot;
        env->HI[0][env->current_tc] = res.rem;
    }
}
