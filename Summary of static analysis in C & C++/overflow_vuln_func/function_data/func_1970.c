typedef struct BDRVVmdkState {
    CoMutex lock;
    int desc_offset;
    bool cid_updated;
    uint32_t parent_cid;
    int num_extents;
    /
    VmdkExtent *extents;
    Error *migration_blocker;
} BDRVVmdkState;
