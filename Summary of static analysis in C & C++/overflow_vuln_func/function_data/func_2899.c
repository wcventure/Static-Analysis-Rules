void ppc6xx_irq_init (CPUState *env)
{
    env->irq_inputs = (void **)qemu_allocate_irqs(&ppc6xx_set_irq, env, 6);
}
