typedef struct BDRVParallelsState {
    CoMutex lock;

    uint32_t *catalog_bitmap;
    int catalog_size;

    int tracks;
} BDRVParallelsState;
