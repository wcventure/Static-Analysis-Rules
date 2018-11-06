static void wiener_denoise(WMAVoiceContext *s, int fcb_type,
                           float *synth_pf, int size,
                           const float *lpcs)
{
    int remainder, lim, n;

    if (fcb_type != FCB_TYPE_SILENCE) {
        float *tilted_lpcs = s->tilted_lpcs_pf,
              *coeffs = s->denoise_coeffs_pf, tilt_mem = 0;

        tilted_lpcs[0]           = 1.0;
        memcpy(&tilted_lpcs[1], lpcs, sizeof(lpcs[0]) * s->lsps);
        memset(&tilted_lpcs[s->lsps + 1], 0,
               sizeof(tilted_lpcs[0]) * (128 - s->lsps - 1));
        ff_tilt_compensation(&tilt_mem, 0.7 * tilt_factor(lpcs, s->lsps),
                             tilted_lpcs, s->lsps + 2);

        /
        remainder = FFMIN(127 - size, size - 1);
        calc_input_response(s, tilted_lpcs, fcb_type, coeffs, remainder);

        /
        memset(&synth_pf[size], 0, sizeof(synth_pf[0]) * (128 - size));
        ff_rdft_calc(&s->rdft, synth_pf);
        ff_rdft_calc(&s->rdft, coeffs);
        synth_pf[0] *= coeffs[0];
        synth_pf[1] *= coeffs[1];
        for (n = 1; n < 128; n++) {
            float v1 = synth_pf[n * 2], v2 = synth_pf[n * 2 + 1];
            synth_pf[n * 2]     = v1 * coeffs[n * 2] - v2 * coeffs[n * 2 + 1];
            synth_pf[n * 2 + 1] = v2 * coeffs[n * 2] + v1 * coeffs[n * 2 + 1];
        }
        ff_rdft_calc(&s->irdft, synth_pf);
    }

    /
    if (s->denoise_filter_cache_size) {
        lim = FFMIN(s->denoise_filter_cache_size, size);
        for (n = 0; n < lim; n++)
            synth_pf[n] += s->denoise_filter_cache[n];
        s->denoise_filter_cache_size -= lim;
        memmove(s->denoise_filter_cache, &s->denoise_filter_cache[size],
                sizeof(s->denoise_filter_cache[0]) * s->denoise_filter_cache_size);
    }

    /
    if (fcb_type != FCB_TYPE_SILENCE) {
        lim = FFMIN(remainder, s->denoise_filter_cache_size);
        for (n = 0; n < lim; n++)
            s->denoise_filter_cache[n] += synth_pf[size + n];
        if (lim < remainder) {
            memcpy(&s->denoise_filter_cache[lim], &synth_pf[size + lim],
                   sizeof(s->denoise_filter_cache[0]) * (remainder - lim));
            s->denoise_filter_cache_size = remainder;
        }
    }
}
