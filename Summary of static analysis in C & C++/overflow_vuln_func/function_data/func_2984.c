typedef struct Flash {
    SSISlave parent_obj;

    BlockBackend *blk;

    uint8_t *storage;
    uint32_t size;
    int page_size;

    uint8_t state;
    uint8_t data[16];
    uint32_t len;
    uint32_t pos;
    uint8_t needed_bytes;
    uint8_t cmd_in_progress;
    uint32_t cur_addr;
    uint32_t nonvolatile_cfg;
    /
    uint32_t volatile_cfg;
    uint32_t enh_volatile_cfg;
    /
    uint8_t spansion_cr1nv;
    uint8_t spansion_cr2nv;
    uint8_t spansion_cr3nv;
    uint8_t spansion_cr4nv;
    uint8_t spansion_cr1v;
    uint8_t spansion_cr2v;
    uint8_t spansion_cr3v;
    uint8_t spansion_cr4v;
    bool write_enable;
    bool four_bytes_address_mode;
    bool reset_enable;
    bool quad_enable;
    uint8_t ear;

    int64_t dirty_page;

    const FlashPartInfo *pi;

} Flash;
