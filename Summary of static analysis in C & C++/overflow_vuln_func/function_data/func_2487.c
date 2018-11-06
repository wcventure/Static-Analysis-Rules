static uint64_t xscom_read(void *opaque, hwaddr addr, unsigned width)
{
    PnvChip *chip = opaque;
    uint32_t pcba = pnv_xscom_pcba(chip, addr);
    uint64_t val = 0;
    MemTxResult result;

    /
    val = xscom_read_default(chip, pcba);
    if (val != -1) {
        goto complete;
    }

    val = address_space_ldq(&chip->xscom_as, pcba << 3, MEMTXATTRS_UNSPECIFIED,
                            &result);
    if (result != MEMTX_OK) {
        qemu_log_mask(LOG_GUEST_ERROR, "XSCOM read failed at @0x%"
                      HWADDR_PRIx " pcba=0x%08x\n", addr, pcba);
        xscom_complete(current_cpu, HMER_XSCOM_FAIL | HMER_XSCOM_DONE);
        return 0;
    }

complete:
    xscom_complete(current_cpu, HMER_XSCOM_DONE);
    return val;
}
