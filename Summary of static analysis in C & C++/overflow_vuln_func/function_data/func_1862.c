int e820_add_entry(uint64_t address, uint64_t length, uint32_t type)
{
    int index = le32_to_cpu(e820_reserve.count);
    struct e820_entry *entry;

    if (type != E820_RAM) {
        /
        if (index >= E820_NR_ENTRIES) {
            return -EBUSY;
        }
        entry = &e820_reserve.entry[index++];

        entry->address = cpu_to_le64(address);
        entry->length = cpu_to_le64(length);
        entry->type = cpu_to_le32(type);

        e820_reserve.count = cpu_to_le32(index);
    }

    /
    e820_table = g_realloc(e820_table,
                           sizeof(struct e820_entry) * (e820_entries+1));
    e820_table[e820_entries].address = cpu_to_le64(address);
    e820_table[e820_entries].length = cpu_to_le64(length);
    e820_table[e820_entries].type = cpu_to_le32(type);
    e820_entries++;

    return e820_entries;
}
