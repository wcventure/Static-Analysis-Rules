#if defined(CONFIG_USER_ONLY) 
int cpu_ppc_handle_mmu_fault (CPUState *env, uint32_t address, int rw,
                              int is_user, int is_softmmu)
{
    int exception, error_code;
    
    if (rw == 2) {
        exception = EXCP_ISI;
        error_code = 0;
    } else {
        exception = EXCP_DSI;
        error_code = 0;
        if (rw)
            error_code |= 0x02000000;
        env->spr[SPR_DAR] = address;
        env->spr[SPR_DSISR] = error_code;
    }
    env->exception_index = exception;
    env->error_code = error_code;

    return 1;
}
