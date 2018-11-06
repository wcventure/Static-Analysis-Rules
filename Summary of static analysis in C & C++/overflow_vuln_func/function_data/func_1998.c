void HELPER(ove)(CPUOpenRISCState *env, target_ulong test)
{
    if (unlikely(test)) {
        OpenRISCCPU *cpu = openrisc_env_get_cpu(env);
        CPUState *cs = CPU(cpu);

        cs->exception_index = EXCP_RANGE;
        cpu_loop_exit_restore(cs, GETPC());
    }
}
