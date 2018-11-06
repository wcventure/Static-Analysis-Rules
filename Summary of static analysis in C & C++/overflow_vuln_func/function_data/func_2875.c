typedef struct QEMU_PACKED QCowSnapshotHeader {
    /
    uint64_t l1_table_offset;

    uint32_t l1_size;
    uint16_t id_str_size;
    uint16_t name_size;

    uint32_t date_sec;
    uint32_t date_nsec;

    uint64_t vm_clock_nsec;

    uint32_t vm_state_size;
    uint32_t extra_data_size; /
    /
    /
    /
} QCowSnapshotHeader;
