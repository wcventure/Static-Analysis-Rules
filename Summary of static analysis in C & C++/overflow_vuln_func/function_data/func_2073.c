typedef struct BDRVQcowState {
    int cluster_bits;
    int cluster_size;
    int cluster_sectors;
    int l2_bits;
    int l2_size;
    int l1_size;
    uint64_t cluster_offset_mask;
    uint64_t l1_table_offset;
    uint64_t *l1_table;
    uint64_t *l2_cache;
    uint64_t l2_cache_offsets[L2_CACHE_SIZE];
    uint32_t l2_cache_counts[L2_CACHE_SIZE];
    uint8_t *cluster_cache;
    uint8_t *cluster_data;
    uint64_t cluster_cache_offset;
    uint32_t crypt_method; /
    uint32_t crypt_method_header;
    AES_KEY aes_encrypt_key;
    AES_KEY aes_decrypt_key;
    CoMutex lock;
    Error *migration_blocker;
} BDRVQcowState;
