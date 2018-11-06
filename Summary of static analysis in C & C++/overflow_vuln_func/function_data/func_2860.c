static void htab_save_first_pass(QEMUFile *f, sPAPRMachineState *spapr,
                                 int64_t max_ns)
{
    bool has_timeout = max_ns != -1;
    int htabslots = HTAB_SIZE(spapr) / HASH_PTE_SIZE_64;
    int index = spapr->htab_save_index;
    int64_t starttime = qemu_clock_get_ns(QEMU_CLOCK_REALTIME);

    assert(spapr->htab_first_pass);

    do {
        int chunkstart;

        /
        while ((index < htabslots)
               && !HPTE_VALID(HPTE(spapr->htab, index))) {
            index++;
            CLEAN_HPTE(HPTE(spapr->htab, index));
        }

        /
        chunkstart = index;
        while ((index < htabslots) && (index - chunkstart < USHRT_MAX)
               && HPTE_VALID(HPTE(spapr->htab, index))) {
            index++;
            CLEAN_HPTE(HPTE(spapr->htab, index));
        }

        if (index > chunkstart) {
            int n_valid = index - chunkstart;

            qemu_put_be32(f, chunkstart);
            qemu_put_be16(f, n_valid);
            qemu_put_be16(f, 0);
            qemu_put_buffer(f, HPTE(spapr->htab, chunkstart),
                            HASH_PTE_SIZE_64 * n_valid);

            if (has_timeout &&
                (qemu_clock_get_ns(QEMU_CLOCK_REALTIME) - starttime) > max_ns) {
                break;
            }
        }
    } while ((index < htabslots) && !qemu_file_rate_limit(f));

    if (index >= htabslots) {
        assert(index == htabslots);
        index = 0;
        spapr->htab_first_pass = false;
    }
    spapr->htab_save_index = index;
}
