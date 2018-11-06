static void pm_update_sci(PIIX4PMState *s)
{
    int sci_level, pmsts;
    int64_t expire_time;

    pmsts = get_pmsts(s);
    sci_level = (((pmsts & s->pmen) &
                  (RTC_EN | PWRBTN_EN | GBL_EN | TMROF_EN)) != 0);
    qemu_set_irq(s->irq, sci_level);
    /
    if ((s->pmen & TMROF_EN) && !(pmsts & TMROF_EN)) {
        expire_time = muldiv64(s->tmr_overflow_time, ticks_per_sec, PM_FREQ);
        qemu_mod_timer(s->tmr_timer, expire_time);
        s->tmr_overflow_time += 0x800000;
    } else {
        qemu_del_timer(s->tmr_timer);
    }
}
