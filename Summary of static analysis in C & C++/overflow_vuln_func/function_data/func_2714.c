static target_ulong h_put_tce(PowerPCCPU *cpu, sPAPREnvironment *spapr,
                              target_ulong opcode, target_ulong *args)
{
    target_ulong liobn = args[0];
    target_ulong ioba = args[1];
    target_ulong tce = args[2];
    sPAPRTCETable *tcet = spapr_tce_find_by_liobn(liobn);

    if (liobn & 0xFFFFFFFF00000000ULL) {
        hcall_dprintf("spapr_vio_put_tce on out-of-boundsw LIOBN "
                      TARGET_FMT_lx "\n", liobn);
        return H_PARAMETER;
    }

    ioba &= ~(SPAPR_TCE_PAGE_SIZE - 1);

    if (tcet) {
        return put_tce_emu(tcet, ioba, tce);
    }
#ifdef DEBUG_TCE
    fprintf(stderr, "%s on liobn=" TARGET_FMT_lx /
            "  ioba 0x" TARGET_FMT_lx "  TCE 0x" TARGET_FMT_lx "\n",
            __func__, liobn, /ioba, tce);
#endif

    return H_PARAMETER;
}
