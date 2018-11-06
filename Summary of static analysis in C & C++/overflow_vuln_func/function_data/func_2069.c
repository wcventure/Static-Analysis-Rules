static void do_io_interrupt(CPUS390XState *env)
{
    S390CPU *cpu = s390_env_get_cpu(env);
    LowCore *lowcore;
    IOIntQueue *q;
    uint8_t isc;
    int disable = 1;
    int found = 0;

    if (!(env->psw.mask & PSW_MASK_IO)) {
        cpu_abort(CPU(cpu), "I/O int w/o I/O mask\n");
    }

    for (isc = 0; isc < ARRAY_SIZE(env->io_index); isc++) {
        uint64_t isc_bits;

        if (env->io_index[isc] < 0) {
            continue;
        }
        if (env->io_index[isc] > MAX_IO_QUEUE) {
            cpu_abort(CPU(cpu), "I/O queue overrun for isc %d: %d\n",
                      isc, env->io_index[isc]);
        }

        q = &env->io_queue[env->io_index[isc]][isc];
        isc_bits = ISC_TO_ISC_BITS(IO_INT_WORD_ISC(q->word));
        if (!(env->cregs[6] & isc_bits)) {
            disable = 0;
            continue;
        }
        if (!found) {
            uint64_t mask, addr;

            found = 1;
            lowcore = cpu_map_lowcore(env);

            lowcore->subchannel_id = cpu_to_be16(q->id);
            lowcore->subchannel_nr = cpu_to_be16(q->nr);
            lowcore->io_int_parm = cpu_to_be32(q->parm);
            lowcore->io_int_word = cpu_to_be32(q->word);
            lowcore->io_old_psw.mask = cpu_to_be64(get_psw_mask(env));
            lowcore->io_old_psw.addr = cpu_to_be64(env->psw.addr);
            mask = be64_to_cpu(lowcore->io_new_psw.mask);
            addr = be64_to_cpu(lowcore->io_new_psw.addr);

            cpu_unmap_lowcore(lowcore);

            env->io_index[isc]--;

            DPRINTF("%s: %" PRIx64 " %" PRIx64 "\n", __func__,
                    env->psw.mask, env->psw.addr);
            load_psw(env, mask, addr);
        }
        if (env->io_index[isc] >= 0) {
            disable = 0;
        }
        continue;
    }

    if (disable) {
        env->pending_int &= ~INTERRUPT_IO;
    }

}
