typedef struct {
    uint32_t version;
    uint32_t flags;
    int64_t capacity;
    int64_t granularity;
    int64_t desc_offset;
    int64_t desc_size;
    int32_t num_gtes_per_gte;
    int64_t rgd_offset;
    int64_t gd_offset;
    int64_t grain_offset;
    char filler[1];
    char check_bytes[4];
    uint16_t compressAlgorithm;
} QEMU_PACKED VMDK4Header;
