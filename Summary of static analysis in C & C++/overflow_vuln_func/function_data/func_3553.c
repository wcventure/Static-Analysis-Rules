static av_cold void compute_alpha_vlcs(void)
{
    uint16_t run_code[129], level_code[256];
    uint8_t run_bits[129], level_bits[256];
    int run, level;

    for (run = 0; run < 128; run++) {
        if (!run) {
            /
            run_code[run] = 0;
            run_bits[run] = 1;
        } else if (run <= 4) {
            /
            run_code[run] = ((run - 1) << 2) | 1;
            run_bits[run] = 4;
        } else {
            /
            run_code[run] = (run << 3) | 7;
            run_bits[run] = 10;
        }
    }

    /
    run_code[128] = 3;
    run_bits[128] = 3;

    INIT_LE_VLC_STATIC(&ff_dc_alpha_run_vlc_le, ALPHA_VLC_BITS, 129,
                       run_bits, 1, 1,
                       run_code, 2, 2, 160);

    for (level = 0; level < 256; level++) {
        int8_t signed_level = (int8_t)level;
        int abs_signed_level = abs(signed_level);
        int sign = (signed_level < 0) ? 1 : 0;

        if (abs_signed_level == 1) {
            /
            level_code[level] = (sign << 1) | 1;
            level_bits[level] = 2;
        } else if (abs_signed_level >= 2 && abs_signed_level <= 5) {
            /
            level_code[level] = ((abs_signed_level - 2) << 3) | (sign << 2) | 2;
            level_bits[level] = 5;
        } else {
            /
            level_code[level] = level << 2;
            level_bits[level] = 10;
        }
    }

    INIT_LE_VLC_STATIC(&ff_dc_alpha_level_vlc_le, ALPHA_VLC_BITS, 256,
                       level_bits, 1, 1,
                       level_code, 2, 2, 288);
}
