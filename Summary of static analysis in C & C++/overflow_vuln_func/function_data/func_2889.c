static void slavio_check_interrupts(SLAVIO_INTCTLState *s, int set_irqs)
{
    uint32_t pending = s->intregm_pending, pil_pending;
    unsigned int i, j;

    pending &= ~s->intregm_disabled;

    trace_slavio_check_interrupts(pending, s->intregm_disabled);
    for (i = 0; i < MAX_CPUS; i++) {
        pil_pending = 0;

        /
        if (pending && !(s->intregm_disabled & MASTER_DISABLE) &&
            (i == s->target_cpu)) {
            for (j = 0; j < 32; j++) {
                if ((pending & (1 << j)) && intbit_to_level[j]) {
                    pil_pending |= 1 << intbit_to_level[j];
                }
            }
        }

        /
        s->slaves[i].intreg_pending &= CPU_SOFTIRQ_MASK | CPU_IRQ_INT15_IN |
            CPU_IRQ_TIMER_IN;
        if (i == s->target_cpu) {
            for (j = 0; j < 32; j++) {
                if ((s->intregm_pending & (1 << j)) && intbit_to_level[j]) {
                    s->slaves[i].intreg_pending |= 1 << intbit_to_level[j];
                }
            }
        }

        /
        if (!(s->intregm_disabled & MASTER_DISABLE)) {
            pil_pending |= s->slaves[i].intreg_pending &
                (CPU_IRQ_INT15_IN | CPU_IRQ_TIMER_IN);
        }

        /
        pil_pending |= (s->slaves[i].intreg_pending & CPU_SOFTIRQ_MASK) >> 16;

        if (set_irqs) {
            for (j = MAX_PILS; j > 0; j--) {
                if (pil_pending & (1 << j)) {
                    if (!(s->slaves[i].irl_out & (1 << j))) {
                        qemu_irq_raise(s->cpu_irqs[i][j]);
                    }
                } else {
                    if (s->slaves[i].irl_out & (1 << j)) {
                        qemu_irq_lower(s->cpu_irqs[i][j]);
                    }
                }
            }
        }
        s->slaves[i].irl_out = pil_pending;
    }
}
