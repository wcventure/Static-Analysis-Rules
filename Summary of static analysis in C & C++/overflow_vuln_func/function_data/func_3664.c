typedef struct Hnm4VideoContext {
    uint8_t version;
    uint16_t width;
    uint16_t height;
    uint8_t *current;
    uint8_t *previous;
    uint8_t *buffer1;
    uint8_t *buffer2;
    uint8_t *processed;
    uint32_t palette[256];
} Hnm4VideoContext;
