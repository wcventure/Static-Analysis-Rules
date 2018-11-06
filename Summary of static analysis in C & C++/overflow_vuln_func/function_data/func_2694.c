void cpu_write_xer(CPUPPCState *env, target_ulong xer)
{
    env->so = (xer >> XER_SO) & 1;
    env->ov = (xer >> XER_OV) & 1;
    env->ca = (xer >> XER_CA) & 1;
    env->xer = xer & ~((1u << XER_SO) | (1u << XER_OV) | (1u << XER_CA));
}
