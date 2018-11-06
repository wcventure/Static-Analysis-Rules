uint16_t acpi_pm1_evt_get_sts(ACPIREGS *ar)
{
    int64_t d = acpi_pm_tmr_get_clock();
    if (d >= ar->tmr.overflow_time) {
        ar->pm1.evt.sts |= ACPI_BITMASK_TIMER_STATUS;
    }
    return ar->pm1.evt.sts;
}
