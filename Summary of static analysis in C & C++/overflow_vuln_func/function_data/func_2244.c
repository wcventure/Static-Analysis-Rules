int cpu_ppc_handle_mmu_fault (CPUState *env, uint32_t address, int rw,
                              int is_user, int is_softmmu)
{
    mmu_ctx_t ctx;
    int exception = 0, error_code = 0;
    int access_type;
    int ret = 0;
    
    if (rw == 2) {
        /
        rw = 0;
        access_type = ACCESS_CODE;
    } else {
        /
        /
        access_type = ACCESS_INT;
        //        access_type = env->access_type;
    }
    ret = get_physical_address(env, &ctx, address, rw, access_type, 1);
    if (ret == 0) {
        ret = tlb_set_page(env, address & TARGET_PAGE_MASK,
                           ctx.raddr & TARGET_PAGE_MASK, ctx.prot,
                           is_user, is_softmmu);
    } else if (ret < 0) {
#if defined (DEBUG_MMU)
        if (loglevel > 0)
            cpu_dump_state(env, logfile, fprintf, 0);
#endif
        if (access_type == ACCESS_CODE) {
            exception = EXCP_ISI;
            switch (ret) {
            case -1:
                /
                if (unlikely(PPC_MMU(env) == PPC_FLAGS_MMU_SOFT_6xx)) {
                    exception = EXCP_I_TLBMISS;
                    env->spr[SPR_IMISS] = address;
                    env->spr[SPR_ICMP] = 0x80000000 | ctx.ptem;
                    error_code = 1 << 18;
                    goto tlb_miss;
                } else if (unlikely(PPC_MMU(env) == PPC_FLAGS_MMU_SOFT_4xx)) {
                    /
                } else {
                    error_code = 0x40000000;
                }
                break;
            case -2:
                /
                error_code = 0x08000000;
                break;
            case -3:
                /
                error_code = 0x10000000;
                break;
            case -4:
                /
                /
                error_code = 0x10000000;
                break;
            case -5:
                /
                exception = EXCP_ISEG;
                error_code = 0;
                break;
            }
        } else {
            exception = EXCP_DSI;
            switch (ret) {
            case -1:
                /
                if (unlikely(PPC_MMU(env) == PPC_FLAGS_MMU_SOFT_6xx)) {
                    if (rw == 1) {
                        exception = EXCP_DS_TLBMISS;
                        error_code = 1 << 16;
                    } else {
                        exception = EXCP_DL_TLBMISS;
                        error_code = 0;
                    }
                    env->spr[SPR_DMISS] = address;
                    env->spr[SPR_DCMP] = 0x80000000 | ctx.ptem;
                tlb_miss:
                    error_code |= ctx.key << 19;
                    env->spr[SPR_HASH1] = ctx.pg_addr[0];
                    env->spr[SPR_HASH2] = ctx.pg_addr[1];
                    /
                    goto out;
                } else if (unlikely(PPC_MMU(env) == PPC_FLAGS_MMU_SOFT_4xx)) {
                    /
                } else {
                    error_code = 0x40000000;
                }
                break;
            case -2:
                /
                error_code = 0x08000000;
                break;
            case -4:
                /
                switch (access_type) {
                case ACCESS_FLOAT:
                    /
                    exception = EXCP_ALIGN;
                    error_code = EXCP_ALIGN_FP;
                    break;
                case ACCESS_RES:
                    /
                    error_code = 0x04000000;
                    break;
                case ACCESS_EXT:
                    /
                    error_code = 0x04100000;
                    break;
                default:
                    printf("DSI: invalid exception (%d)\n", ret);
                    exception = EXCP_PROGRAM;
                    error_code = EXCP_INVAL | EXCP_INVAL_INVAL;
                    break;
                }
                break;
            case -5:
                /
                exception = EXCP_DSEG;
                error_code = 0;
                break;
            }
            if (exception == EXCP_DSI && rw == 1)
                error_code |= 0x02000000;
            /
            env->spr[SPR_DAR] = address;
            env->spr[SPR_DSISR] = error_code;
        }
    out:
#if 0
        printf("%s: set exception to %d %02x\n",
               __func__, exception, error_code);
#endif
        env->exception_index = exception;
        env->error_code = error_code;
        ret = 1;
    }

    return ret;
}
