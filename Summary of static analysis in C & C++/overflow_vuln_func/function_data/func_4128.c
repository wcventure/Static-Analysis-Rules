typedef struct FlacSubframe {
    int type;
    int type_code;
    int obits;
    int order;
    int32_t coefs[MAX_LPC_ORDER];
    int shift;
    RiceContext rc;
    int32_t samples[FLAC_MAX_BLOCKSIZE];
    int32_t residual[FLAC_MAX_BLOCKSIZE];
} FlacSubframe;
