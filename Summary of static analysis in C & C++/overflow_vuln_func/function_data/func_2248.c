static void dump_syscall(CPUState *env)
{
    fprintf(logfile, "syscall r0=0x%08x r3=0x%08x r4=0x%08x "
            "r5=0x%08x r6=0x%08x nip=0x%08x\n",
            env->gpr[0], env->gpr[3], env->gpr[4],
            env->gpr[5], env->gpr[6], env->nip);
}
