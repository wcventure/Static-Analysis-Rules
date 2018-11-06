target_ulong helper_dvpe(CPUMIPSState *env)
{
    CPUMIPSState *other_cpu = first_cpu;
    target_ulong prev = env->mvp->CP0_MVPControl;

    do {
        /
        if (other_cpu != env) {
            other_cpu->mvp->CP0_MVPControl &= ~(1 << CP0MVPCo_EVP);
            mips_vpe_sleep(other_cpu);
        }
        other_cpu = other_cpu->next_cpu;
    } while (other_cpu);
    return prev;
}
