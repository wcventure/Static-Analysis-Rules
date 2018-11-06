void do_load_6xx_tlb (int is_code)
{
    target_ulong RPN, CMP, EPN;
    int way;
    
    RPN = env->spr[SPR_RPA];
    if (is_code) {
        CMP = env->spr[SPR_ICMP];
        EPN = env->spr[SPR_IMISS];
    } else {
        CMP = env->spr[SPR_DCMP];
        EPN = env->spr[SPR_DMISS];
    }
    way = (env->spr[SPR_SRR1] >> 17) & 1;
#if defined (DEBUG_SOFTWARE_TLB)
    if (loglevel != 0) {
        fprintf(logfile, "%s: EPN %08lx %08lx PTE0 %08lx PTE1 %08lx way %d\n",
                __func__, (unsigned long)T0, (unsigned long)EPN,
                (unsigned long)CMP, (unsigned long)RPN, way);
    }
#endif
    /
    ppc6xx_tlb_store(env, T0 & TARGET_PAGE_MASK, way, is_code, CMP, RPN);
}
