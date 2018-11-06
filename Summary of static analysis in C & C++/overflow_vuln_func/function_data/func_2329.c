#if !defined (CONFIG_USER_ONLY)
void do_rfi (void)
{
    env->nip = env->spr[SPR_SRR0] & ~0x00000003;
    T0 = env->spr[SPR_SRR1] & ~0xFFFF0000UL;
    do_store_msr(env, T0);
#if defined (DEBUG_OP)
    dump_rfi();
#endif
    env->interrupt_request |= CPU_INTERRUPT_EXITTB;
}
