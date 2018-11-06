typedef struct MirrorBlockJob {
    BlockJob common;
    RateLimit limit;
    BlockBackend *target;
    BlockDriverState *base;
    /
    char *replaces;
    /
    BlockDriverState *to_replace;
    /
    Error *replace_blocker;
    bool is_none_mode;
    BlockMirrorBackingMode backing_mode;
    BlockdevOnError on_source_error, on_target_error;
    bool synced;
    bool should_complete;
    int64_t granularity;
    size_t buf_size;
    int64_t bdev_length;
    unsigned long *cow_bitmap;
    BdrvDirtyBitmap *dirty_bitmap;
    HBitmapIter hbi;
    uint8_t *buf;
    QSIMPLEQ_HEAD(, MirrorBuffer) buf_free;
    int buf_free_count;

    unsigned long *in_flight_bitmap;
    int in_flight;
    int sectors_in_flight;
    int ret;
    bool unmap;
    bool waiting_for_io;
    int target_cluster_sectors;
    int max_iov;
} MirrorBlockJob;
