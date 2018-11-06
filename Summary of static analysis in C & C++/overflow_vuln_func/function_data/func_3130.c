static av_always_inline int sbr_hf_apply_noise(int (*Y)[2],
                                                const SoftFloat *s_m,
                                                const SoftFloat *q_filt,
                                                int noise,
                                                int phi_sign0,
                                                int phi_sign1,
                                                int m_max)
{
    int m;

    for (m = 0; m < m_max; m++) {
        int y0 = Y[m][0];
        int y1 = Y[m][1];
        noise = (noise + 1) & 0x1ff;
        if (s_m[m].mant) {
            int shift, round;

            shift = 22 - s_m[m].exp;
            if (shift < 1) {
                av_log(NULL, AV_LOG_ERROR, "Overflow in sbr_hf_apply_noise, shift=%d\n", shift);
                return AVERROR(ERANGE);
            } else if (shift < 30) {
                round = 1 << (shift-1);
                y0 += (s_m[m].mant * phi_sign0 + round) >> shift;
                y1 += (s_m[m].mant * phi_sign1 + round) >> shift;
            }
        } else {
            int shift, round, tmp;
            int64_t accu;

            shift = 22 - q_filt[m].exp;
            if (shift < 1) {
                av_log(NULL, AV_LOG_ERROR, "Overflow in sbr_hf_apply_noise, shift=%d\n", shift);
                return AVERROR(ERANGE);
            } else if (shift < 30) {
                round = 1 << (shift-1);

                accu = (int64_t)q_filt[m].mant * ff_sbr_noise_table_fixed[noise][0];
                tmp = (int)((accu + 0x40000000) >> 31);
                y0 += (tmp + round) >> shift;

                accu = (int64_t)q_filt[m].mant * ff_sbr_noise_table_fixed[noise][1];
                tmp = (int)((accu + 0x40000000) >> 31);
                y1 += (tmp + round) >> shift;
            }
        }
        Y[m][0] = y0;
        Y[m][1] = y1;
        phi_sign1 = -phi_sign1;
    }
    return 0;
}
