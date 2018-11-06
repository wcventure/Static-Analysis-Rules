typedef struct BDRVVVFATState {
    CoMutex lock;
    BlockDriverState* bs; /
    unsigned char first_sectors[0x40*0x200];

    int fat_type; /
    array_t fat,directory,mapping;
    char volume_label[11];

    uint32_t offset_to_bootsector; /

    unsigned int cluster_size;
    unsigned int sectors_per_cluster;
    unsigned int sectors_per_fat;
    unsigned int sectors_of_root_directory;
    uint32_t last_cluster_of_root_directory;
    uint32_t sector_count; /
    uint32_t cluster_count; /
    uint32_t max_fat_value;
    uint32_t offset_to_fat;
    uint32_t offset_to_root_dir;

    int current_fd;
    mapping_t* current_mapping;
    unsigned char* cluster; /
    unsigned char* cluster_buffer; /
    unsigned int current_cluster;

    /
    char* qcow_filename;
    BdrvChild* qcow;
    void* fat2;
    char* used_clusters;
    array_t commits;
    const char* path;
    int downcase_short_names;

    Error *migration_blocker;
} BDRVVVFATState;
