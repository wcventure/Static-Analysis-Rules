void acpi_pm1_evt_write_sts(ACPIREGS *ar, uint16_t val)
{
    uint16_t pm1_sts = acpi_pm1_evt_get_sts(ar, ar->tmr.overflow_time);
    if (pm1_sts & val & ACPI_BITMASK_TIMER_STATUS) {
        /
        acpi_pm_tmr_calc_overflow_time(ar);
    }
    ar->pm1.evt.sts &= ~val;
}
