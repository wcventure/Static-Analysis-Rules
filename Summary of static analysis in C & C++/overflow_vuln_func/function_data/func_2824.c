int mmu_translate(CPUS390XState *env, target_ulong vaddr, int rw, uint64_t asc,
                  target_ulong *raddr, int *flags, bool exc)
{
    int r = -1;
    uint8_t *sk;

    *flags = PAGE_READ | PAGE_WRITE | PAGE_EXEC;
    vaddr &= TARGET_PAGE_MASK;

    if (!(env->psw.mask & PSW_MASK_DAT)) {
        *raddr = vaddr;
        r = 0;
        goto out;
    }

    switch (asc) {
    case PSW_ASC_PRIMARY:
        PTE_DPRINTF("%s: asc=primary\n", __func__);
        r = mmu_translate_asce(env, vaddr, asc, env->cregs[1], raddr, flags,
                               rw, exc);
        break;
    case PSW_ASC_HOME:
        PTE_DPRINTF("%s: asc=home\n", __func__);
        r = mmu_translate_asce(env, vaddr, asc, env->cregs[13], raddr, flags,
                               rw, exc);
        break;
    case PSW_ASC_SECONDARY:
        PTE_DPRINTF("%s: asc=secondary\n", __func__);
        /
        if (rw == MMU_INST_FETCH) {
            r = mmu_translate_asce(env, vaddr, PSW_ASC_PRIMARY, env->cregs[1],
                                   raddr, flags, rw, exc);
            *flags &= ~(PAGE_READ | PAGE_WRITE);
        } else {
            r = mmu_translate_asce(env, vaddr, PSW_ASC_SECONDARY, env->cregs[7],
                                   raddr, flags, rw, exc);
            *flags &= ~(PAGE_EXEC);
        }
        break;
    case PSW_ASC_ACCREG:
    default:
        hw_error("guest switched to unknown asc mode\n");
        break;
    }

 out:
    /
    *raddr = mmu_real2abs(env, *raddr);

    if (*raddr <= ram_size) {
        sk = &env->storage_keys[*raddr / TARGET_PAGE_SIZE];
        if (*flags & PAGE_READ) {
            *sk |= SK_R;
        }

        if (*flags & PAGE_WRITE) {
            *sk |= SK_C;
        }
    }

    return r;
}
