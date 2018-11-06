static int ppc6xx_tlb_check (CPUState *env, mmu_ctx_t *ctx,
                             target_ulong eaddr, int rw, int access_type)
{
    ppc_tlb_t *tlb;
    int nr, best, way;
    int ret;
    
    best = -1;
    ret = -1; /
    for (way = 0; way < env->nb_ways; way++) {
        nr = ppc6xx_tlb_getnum(env, eaddr, way,
                               access_type == ACCESS_CODE ? 1 : 0);
        tlb = &env->tlb[nr];
        /
        if ((eaddr & TARGET_PAGE_MASK) != tlb->EPN) {
#if defined (DEBUG_SOFTWARE_TLB)
            if (loglevel != 0) {
                fprintf(logfile, "TLB %d/%d %s [%08x %08x] <> %08x\n",
                        nr, env->nb_tlb,
                        pte_is_valid(tlb->pte0) ? "valid" : "inval",
                        tlb->EPN, tlb->EPN + TARGET_PAGE_SIZE, eaddr);
            }
#endif
            continue;
        }
#if defined (DEBUG_SOFTWARE_TLB)
        if (loglevel != 0) {
            fprintf(logfile, "TLB %d/%d %s %08x <> %08x %08x %c %c\n",
                    nr, env->nb_tlb,
                    pte_is_valid(tlb->pte0) ? "valid" : "inval",
                    tlb->EPN, eaddr, tlb->pte1,
                    rw ? 'S' : 'L', access_type == ACCESS_CODE ? 'I' : 'D');
        }
#endif
        switch (pte_check(ctx, tlb->pte0, tlb->pte1, 0, rw)) {
        case -3:
            /
            return -1;
        case -2:
            /
            ret = -2;
            best = nr;
            break;
        case -1:
        default:
            /
            break;
        case 0:
            /
            /
            ret = 0;
            best = nr;
            goto done;
        }
    }
    if (best != -1) {
    done:
#if defined (DEBUG_SOFTWARE_TLB)
        if (loglevel > 0) {
            fprintf(logfile, "found TLB at addr 0x%08lx prot=0x%01x ret=%d\n",
                    ctx->raddr & TARGET_PAGE_MASK, ctx->prot, ret);
        }
#endif
        /
        pte_update_flags(ctx, &env->tlb[best].pte1, ret, rw);
    }

    return ret;
}
