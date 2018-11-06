int get_physical_address (CPUState *env, mmu_ctx_t *ctx, target_ulong eaddr,
                          int rw, int access_type, int check_BATs)
{
    int ret;
#if 0
int get_physical_address (CPUState *env, mmu_ctx_t *ctx, target_ulong eaddr,
                          int rw, int access_type, int check_BATs)
{
    int ret;
#if 0
    if (loglevel > 0) {
        fprintf(logfile, "%s\n", __func__);
    }
#endif    
    if ((access_type == ACCESS_CODE && msr_ir == 0) ||
        (access_type != ACCESS_CODE && msr_dr == 0)) {
        /
        ret = check_physical(env, ctx, eaddr, rw);
    } else {
        /
        ret = -1;
        if (check_BATs)
            ret = get_bat(env, ctx, eaddr, rw, access_type);
        if (ret < 0) {
            /
            ret = get_segment(env, ctx, eaddr, rw, access_type);
        }
    }
#if 0
            ret = get_segment(env, ctx, eaddr, rw, access_type);
        }
    }
#if 0
    if (loglevel > 0) {
        fprintf(logfile, "%s address %08x => %08lx\n",
                __func__, eaddr, ctx->raddr);
    }
#endif
    
    return ret;
}
