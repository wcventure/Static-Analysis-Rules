static void do_ext_interrupt(CPUS390XState *env)
{
    S390CPU *cpu = s390_env_get_cpu(env);
    uint64_t mask, addr;
    LowCore *lowcore;
    ExtQueue *q;

    if (!(env->psw.mask & PSW_MASK_EXT)) {
        cpu_abort(CPU(cpu), "Ext int w/o ext mask\n");
    }

    if (env->ext_index < 0 || env->ext_index > MAX_EXT_QUEUE) {
        cpu_abort(CPU(cpu), "Ext queue overrun: %d\n", env->ext_index);
    }

    q = &env->ext_queue[env->ext_index];
    lowcore = cpu_map_lowcore(env);

    lowcore->ext_int_code = cpu_to_be16(q->code);
    lowcore->ext_params = cpu_to_be32(q->param);
    lowcore->ext_params2 = cpu_to_be64(q->param64);
    lowcore->external_old_psw.mask = cpu_to_be64(get_psw_mask(env));
    lowcore->external_old_psw.addr = cpu_to_be64(env->psw.addr);
    lowcore->cpu_addr = cpu_to_be16(env->cpu_num | VIRTIO_SUBCODE_64);
    mask = be64_to_cpu(lowcore->external_new_psw.mask);
    addr = be64_to_cpu(lowcore->external_new_psw.addr);

    cpu_unmap_lowcore(lowcore);

    env->ext_index--;
    if (env->ext_index == -1) {
        env->pending_int &= ~INTERRUPT_EXT;
    }

    DPRINTF("%s: %" PRIx64 " %" PRIx64 "\n", __func__,
            env->psw.mask, env->psw.addr);

    load_psw(env, mask, addr);
}
