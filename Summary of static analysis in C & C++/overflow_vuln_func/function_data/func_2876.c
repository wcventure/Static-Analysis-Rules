typedef struct QEMU_PACKED QCowSnapshotExtraData {
    uint64_t vm_state_size_large;
    uint64_t disk_size;
} QCowSnapshotExtraData;
