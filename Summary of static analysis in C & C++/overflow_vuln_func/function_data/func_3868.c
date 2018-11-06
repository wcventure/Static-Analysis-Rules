typedef struct BlockInfo {
    const uint32_t *factor_table;
    const uint8_t *scan_table;
    uint8_t pos; /
    void (*idct_put)(uint8_t *dest, int line_size, DCTELEM *block);
    uint8_t partial_bit_count;
    uint16_t partial_bit_buffer;
    int shift_offset;
} BlockInfo;
