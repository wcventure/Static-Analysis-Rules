static void iommu_config_write(void *opaque, hwaddr addr,
                               uint64_t val, unsigned size)
{
    IOMMUState *is = opaque;

    IOMMU_DPRINTF("IOMMU config write: 0x%" HWADDR_PRIx " val: %" PRIx64
                  " size: %d\n", addr, val, size);

    switch (addr) {
    case IOMMU_CTRL:
        if (size == 4) {
            is->regs[IOMMU_CTRL >> 3] &= 0xffffffffULL;
            is->regs[IOMMU_CTRL >> 3] |= val << 32;
        } else {
            is->regs[IOMMU_CTRL] = val;
        }
        break;
    case IOMMU_CTRL + 0x4:
        is->regs[IOMMU_CTRL >> 3] &= 0xffffffff00000000ULL;
        is->regs[IOMMU_CTRL >> 3] |= val & 0xffffffffULL;
        break;
    case IOMMU_BASE:
        if (size == 4) {
            is->regs[IOMMU_BASE >> 3] &= 0xffffffffULL;
            is->regs[IOMMU_BASE >> 3] |= val << 32;
        } else {
            is->regs[IOMMU_BASE] = val;
        }
        break;
    case IOMMU_BASE + 0x4:
        is->regs[IOMMU_BASE >> 3] &= 0xffffffff00000000ULL;
        is->regs[IOMMU_BASE >> 3] |= val & 0xffffffffULL;
        break;
    default:
        qemu_log_mask(LOG_UNIMP,
                  "apb iommu: Unimplemented register write "
                  "reg 0x%" HWADDR_PRIx " size 0x%x value 0x%" PRIx64 "\n",
                  addr, size, val);
        break;
    }
}
