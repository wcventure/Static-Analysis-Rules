typedef struct {
    int h;          ///< output slice height
    int vshift;     ///< vertical chroma subsampling shift
    uint32_t lcg_state; ///< LCG state used to compute random slice height
    int use_random_h;   ///< enable the use of random slice height values
} SliceContext;
