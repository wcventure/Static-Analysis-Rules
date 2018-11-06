void sparc64_get_context(CPUSPARCState *env)
{
    abi_ulong ucp_addr;
    struct target_ucontext *ucp;
    target_mc_gregset_t *grp;
    target_mcontext_t *mcp;
    abi_ulong fp, i7, w_addr;
    int err;
    unsigned int i;
    target_sigset_t target_set;
    sigset_t set;

    ucp_addr = env->regwptr[UREG_I0];
    if (!lock_user_struct(VERIFY_WRITE, ucp, ucp_addr, 0)) {
        goto do_sigsegv;
    }
    
    mcp = &ucp->tuc_mcontext;
    grp = &mcp->mc_gregs;

    /
    env->pc = env->npc;
    env->npc += 4;

    err = 0;

    do_sigprocmask(0, NULL, &set);
    host_to_target_sigset_internal(&target_set, &set);
    if (TARGET_NSIG_WORDS == 1) {
        __put_user(target_set.sig[0],
                   (abi_ulong *)&ucp->tuc_sigmask);
    } else {
        abi_ulong *src, *dst;
        src = target_set.sig;
        dst = ucp->tuc_sigmask.sig;
        for (i = 0; i < TARGET_NSIG_WORDS; i++, dst++, src++) {
            __put_user(*src, dst);
        }
        if (err)
            goto do_sigsegv;
    }

    /
    //    __put_user(env->tstate, &((*grp)[MC_TSTATE]));
    __put_user(env->pc, &((*grp)[MC_PC]));
    __put_user(env->npc, &((*grp)[MC_NPC]));
    __put_user(env->y, &((*grp)[MC_Y]));
    __put_user(env->gregs[1], &((*grp)[MC_G1]));
    __put_user(env->gregs[2], &((*grp)[MC_G2]));
    __put_user(env->gregs[3], &((*grp)[MC_G3]));
    __put_user(env->gregs[4], &((*grp)[MC_G4]));
    __put_user(env->gregs[5], &((*grp)[MC_G5]));
    __put_user(env->gregs[6], &((*grp)[MC_G6]));
    __put_user(env->gregs[7], &((*grp)[MC_G7]));
    __put_user(env->regwptr[UREG_I0], &((*grp)[MC_O0]));
    __put_user(env->regwptr[UREG_I1], &((*grp)[MC_O1]));
    __put_user(env->regwptr[UREG_I2], &((*grp)[MC_O2]));
    __put_user(env->regwptr[UREG_I3], &((*grp)[MC_O3]));
    __put_user(env->regwptr[UREG_I4], &((*grp)[MC_O4]));
    __put_user(env->regwptr[UREG_I5], &((*grp)[MC_O5]));
    __put_user(env->regwptr[UREG_I6], &((*grp)[MC_O6]));
    __put_user(env->regwptr[UREG_I7], &((*grp)[MC_O7]));

    w_addr = TARGET_STACK_BIAS+env->regwptr[UREG_I6];
    fp = i7 = 0;
    if (get_user(fp, w_addr + offsetof(struct target_reg_window, ins[6]),
                 abi_ulong) != 0) {
        goto do_sigsegv;
    }
    if (get_user(i7, w_addr + offsetof(struct target_reg_window, ins[7]),
                 abi_ulong) != 0) {
        goto do_sigsegv;
    }
    __put_user(fp, &(mcp->mc_fp));
    __put_user(i7, &(mcp->mc_i7));

    {
        uint32_t *dst = ucp->tuc_mcontext.mc_fpregs.mcfpu_fregs.sregs;
        for (i = 0; i < 64; i++, dst++) {
            if (i & 1) {
                __put_user(env->fpr[i/2].l.lower, dst);
            } else {
                __put_user(env->fpr[i/2].l.upper, dst);
            }
        }
    }
    __put_user(env->fsr, &(mcp->mc_fpregs.mcfpu_fsr));
    __put_user(env->gsr, &(mcp->mc_fpregs.mcfpu_gsr));
    __put_user(env->fprs, &(mcp->mc_fpregs.mcfpu_fprs));

    if (err)
        goto do_sigsegv;
    unlock_user_struct(ucp, ucp_addr, 1);
    return;
do_sigsegv:
    unlock_user_struct(ucp, ucp_addr, 1);
    force_sig(TARGET_SIGSEGV);
}
