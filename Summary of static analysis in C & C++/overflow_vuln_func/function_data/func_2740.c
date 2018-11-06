static void raise_exception(CPUM68KState *env, int tt)
{
    CPUState *cs = CPU(m68k_env_get_cpu(env));

    cs->exception_index = tt;
    cpu_loop_exit(cs);
}
