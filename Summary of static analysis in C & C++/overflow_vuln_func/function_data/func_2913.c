static uint64_t itc_tag_read(void *opaque, hwaddr addr, unsigned size)
{
    MIPSITUState *tag = (MIPSITUState *)opaque;
    uint64_t index = addr >> 3;
    uint64_t ret = 0;

    switch (index) {
    case 0 ... ITC_ADDRESSMAP_NUM:
        ret = tag->ITCAddressMap[index];
        break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "Read 0x%" PRIx64 "\n", addr);
        break;
    }

    return ret;
}
