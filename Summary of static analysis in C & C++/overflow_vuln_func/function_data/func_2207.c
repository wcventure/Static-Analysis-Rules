typedef struct DisasContext {
    const XtensaConfig *config;
    TranslationBlock *tb;
    uint32_t pc;
    uint32_t next_pc;
    int cring;
    int ring;
    uint32_t lbeg;
    uint32_t lend;
    TCGv_i32 litbase;
    int is_jmp;
    int singlestep_enabled;

    bool sar_5bit;
    bool sar_m32_5bit;
    bool sar_m32_allocated;
    TCGv_i32 sar_m32;

    uint32_t ccount_delta;
    unsigned used_window;

    bool debug;
    bool icount;
    TCGv_i32 next_icount;

    unsigned cpenable;
} DisasContext;
