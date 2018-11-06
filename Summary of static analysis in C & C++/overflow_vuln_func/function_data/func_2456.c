static inline void elf_core_copy_regs(target_elf_gregset_t *regs,
                                      const CPUSH4State *env)
{
    int i;

    for (i = 0; i < 16; i++) {
        (*regs[i]) = tswapreg(env->gregs[i]);
    }

    (*regs)[TARGET_REG_PC] = tswapreg(env->pc);
    (*regs)[TARGET_REG_PR] = tswapreg(env->pr);
    (*regs)[TARGET_REG_SR] = tswapreg(env->sr);
    (*regs)[TARGET_REG_GBR] = tswapreg(env->gbr);
    (*regs)[TARGET_REG_MACH] = tswapreg(env->mach);
    (*regs)[TARGET_REG_MACL] = tswapreg(env->macl);
    (*regs)[TARGET_REG_SYSCALL] = 0; /
}
