void ppc970_irq_init (CPUState *env)
{
    env->irq_inputs = (void **)qemu_allocate_irqs(&ppc970_set_irq, env, 7);
}
