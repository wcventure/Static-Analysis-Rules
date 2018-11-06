static target_ulong put_tce_emu(sPAPRTCETable *tcet, target_ulong ioba,
                                target_ulong tce)
{
    sPAPRTCE *tcep;

    if (ioba >= tcet->window_size) {
        hcall_dprintf("spapr_vio_put_tce on out-of-boards IOBA 0x"
                      TARGET_FMT_lx "\n", ioba);
        return H_PARAMETER;
    }

    tcep = tcet->table + (ioba >> SPAPR_TCE_PAGE_SHIFT);
    tcep->tce = tce;

    return H_SUCCESS;
}
