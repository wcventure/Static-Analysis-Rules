typedef struct AC3Block {
    uint8_t  **bap;                             ///< bit allocation pointers (bap)
    CoefType **mdct_coef;                       ///< MDCT coefficients
    int32_t  **fixed_coef;                      ///< fixed-point MDCT coefficients
    uint8_t  **exp;                             ///< original exponents
    uint8_t  **grouped_exp;                     ///< grouped exponents
    int16_t  **psd;                             ///< psd per frequency bin
    int16_t  **band_psd;                        ///< psd per critical band
    int16_t  **mask;                            ///< masking curve
    uint16_t **qmant;                           ///< quantized mantissas
    int8_t   exp_shift[AC3_MAX_CHANNELS];       ///< exponent shift values
    uint8_t  new_rematrixing_strategy;          ///< send new rematrixing flags in this block
    uint8_t  rematrixing_flags[4];              ///< rematrixing flags
} AC3Block;
