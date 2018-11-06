static void do_mchk_interrupt(CPUS390XState *env)
{
    S390CPU *cpu = s390_env_get_cpu(env);
    uint64_t mask, addr;
    LowCore *lowcore;
    MchkQueue *q;
    int i;

    if (!(env->psw.mask & PSW_MASK_MCHECK)) {
        cpu_abort(CPU(cpu), "Machine check w/o mchk mask\n");
    }

    if (env->mchk_index < 0 || env->mchk_index > MAX_MCHK_QUEUE) {
        cpu_abort(CPU(cpu), "Mchk queue overrun: %d\n", env->mchk_index);
    }

    q = &env->mchk_queue[env->mchk_index];

    if (q->type != 1) {
        /
        cpu_abort(CPU(cpu), "Unknown machine check type %d\n", q->type);
    }
    if (!(env->cregs[14] & (1 << 28))) {
        /
        return;
    }

    lowcore = cpu_map_lowcore(env);

    for (i = 0; i < 16; i++) {
        lowcore->floating_pt_save_area[i] = cpu_to_be64(env->fregs[i].ll);
        lowcore->gpregs_save_area[i] = cpu_to_be64(env->regs[i]);
        lowcore->access_regs_save_area[i] = cpu_to_be32(env->aregs[i]);
        lowcore->cregs_save_area[i] = cpu_to_be64(env->cregs[i]);
    }
    lowcore->prefixreg_save_area = cpu_to_be32(env->psa);
    lowcore->fpt_creg_save_area = cpu_to_be32(env->fpc);
    lowcore->tod_progreg_save_area = cpu_to_be32(env->todpr);
    lowcore->cpu_timer_save_area[0] = cpu_to_be32(env->cputm >> 32);
    lowcore->cpu_timer_save_area[1] = cpu_to_be32((uint32_t)env->cputm);
    lowcore->clock_comp_save_area[0] = cpu_to_be32(env->ckc >> 32);
    lowcore->clock_comp_save_area[1] = cpu_to_be32((uint32_t)env->ckc);

    lowcore->mcck_interruption_code[0] = cpu_to_be32(0x00400f1d);
    lowcore->mcck_interruption_code[1] = cpu_to_be32(0x40330000);
    lowcore->mcck_old_psw.mask = cpu_to_be64(get_psw_mask(env));
    lowcore->mcck_old_psw.addr = cpu_to_be64(env->psw.addr);
    mask = be64_to_cpu(lowcore->mcck_new_psw.mask);
    addr = be64_to_cpu(lowcore->mcck_new_psw.addr);

    cpu_unmap_lowcore(lowcore);

    env->mchk_index--;
    if (env->mchk_index == -1) {
        env->pending_int &= ~INTERRUPT_MCHK;
    }

    DPRINTF("%s: %" PRIx64 " %" PRIx64 "\n", __func__,
            env->psw.mask, env->psw.addr);

    load_psw(env, mask, addr);
}
