#if defined(TARGET_I386)
static void print_pte(Monitor *mon, uint32_t addr, uint32_t pte, uint32_t mask)
{
    monitor_printf(mon, "%08x: %08x %c%c%c%c%c%c%c%c\n",
                   addr,
                   pte & mask,
                   pte & PG_GLOBAL_MASK ? 'G' : '-',
                   pte & PG_PSE_MASK ? 'P' : '-',
                   pte & PG_DIRTY_MASK ? 'D' : '-',
                   pte & PG_ACCESSED_MASK ? 'A' : '-',
                   pte & PG_PCD_MASK ? 'C' : '-',
                   pte & PG_PWT_MASK ? 'T' : '-',
                   pte & PG_USER_MASK ? 'U' : '-',
                   pte & PG_RW_MASK ? 'W' : '-');
}
