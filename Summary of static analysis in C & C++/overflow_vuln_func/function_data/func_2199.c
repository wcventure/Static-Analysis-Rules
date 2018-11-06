static void qtest_irq_handler(void *opaque, int n, int level)
{
    qemu_irq old_irq = *(qemu_irq *)opaque;
    qemu_set_irq(old_irq, level);

    if (irq_levels[n] != level) {
        CharDriverState *chr = qtest_chr;
        irq_levels[n] = level;
        qtest_send_prefix(chr);
        qtest_send(chr, "IRQ %s %d\n",
                   level ? "raise" : "lower", n);
    }
}
