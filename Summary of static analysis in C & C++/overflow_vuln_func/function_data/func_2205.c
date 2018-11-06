typedef struct SuperIOConfig
{
    uint8_t config[0xff];
    uint8_t index;
    uint8_t data;
} SuperIOConfig;
