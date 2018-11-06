typedef struct BDRVBochsState {
    CoMutex lock;
    uint32_t *catalog_bitmap;
    int catalog_size;

    int data_offset;

    int bitmap_blocks;
    int extent_blocks;
    int extent_size;
} BDRVBochsState;
