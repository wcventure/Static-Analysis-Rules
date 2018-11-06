static void formant_postfilter(G723_1_Context *p, int16_t *lpc, int16_t *buf)
{
    int16_t filter_coef[2][LPC_ORDER], *buf_ptr;
    int filter_signal[LPC_ORDER + FRAME_LEN], *signal_ptr;
    int i, j, k;

    memcpy(buf, p->fir_mem, LPC_ORDER * sizeof(*buf));
    memcpy(filter_signal, p->iir_mem, LPC_ORDER * sizeof(*filter_signal));

    for (i = LPC_ORDER, j = 0; j < SUBFRAMES; i += SUBFRAME_LEN, j++) {
        for (k = 0; k < LPC_ORDER; k++) {
            filter_coef[0][k] = (-lpc[k] * postfilter_tbl[0][k] +
                                 (1 << 14)) >> 15;
            filter_coef[1][k] = (-lpc[k] * postfilter_tbl[1][k] +
                                 (1 << 14)) >> 15;
        }
        iir_filter(filter_coef[0], filter_coef[1], buf + i,
                   filter_signal + i);
        lpc += LPC_ORDER;
    }

    memcpy(p->fir_mem, buf + FRAME_LEN, LPC_ORDER * sizeof(*p->fir_mem));
    memcpy(p->iir_mem, filter_signal + FRAME_LEN,
           LPC_ORDER * sizeof(*p->iir_mem));

    buf_ptr    = buf + LPC_ORDER;
    signal_ptr = filter_signal + LPC_ORDER;
    for (i = 0; i < SUBFRAMES; i++) {
        int16_t temp_vector[SUBFRAME_LEN];
        int16_t temp;
        int auto_corr[2];
        int scale, energy;

        /
        memcpy(temp_vector, buf_ptr, SUBFRAME_LEN * sizeof(*temp_vector));
        scale = scale_vector(temp_vector, SUBFRAME_LEN);

        /
        auto_corr[0] = dot_product(temp_vector, temp_vector + 1,
                                   SUBFRAME_LEN - 1, 1);
        auto_corr[1] = dot_product(temp_vector, temp_vector, SUBFRAME_LEN, 1);

        /
        temp = auto_corr[1] >> 16;
        if (temp) {
            temp = (auto_corr[0] >> 2) / temp;
        }
        p->reflection_coef = (3 * p->reflection_coef + temp + 2) >> 2;
        temp = (p->reflection_coef * 0xffffc >> 3) & 0xfffc;

        /
        for (j = 0; j < SUBFRAME_LEN; j++) {
            buf_ptr[j] = av_clipl_int32(signal_ptr[j] +
                                        ((signal_ptr[j - 1] >> 16) *
                                         temp << 1)) >> 16;
        }

        /
        temp = 2 * scale + 4;
        if (temp < 0) {
            energy = av_clipl_int32((int64_t)auto_corr[1] << -temp);
        } else
            energy = auto_corr[1] >> temp;

        gain_scale(p, buf_ptr, energy);

        buf_ptr    += SUBFRAME_LEN;
        signal_ptr += SUBFRAME_LEN;
    }
}
