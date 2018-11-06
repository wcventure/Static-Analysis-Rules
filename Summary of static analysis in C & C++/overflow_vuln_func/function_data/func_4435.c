static void decorrelation(PSContext *ps, INTFLOAT (*out)[32][2], const INTFLOAT (*s)[32][2], int is34)
{
    LOCAL_ALIGNED_16(INTFLOAT, power, [34], [PS_QMF_TIME_SLOTS]);
    LOCAL_ALIGNED_16(INTFLOAT, transient_gain, [34], [PS_QMF_TIME_SLOTS]);
    INTFLOAT *peak_decay_nrg = ps->peak_decay_nrg;
    INTFLOAT *power_smooth = ps->power_smooth;
    INTFLOAT *peak_decay_diff_smooth = ps->peak_decay_diff_smooth;
    INTFLOAT (*delay)[PS_QMF_TIME_SLOTS + PS_MAX_DELAY][2] = ps->delay;
    INTFLOAT (*ap_delay)[PS_AP_LINKS][PS_QMF_TIME_SLOTS + PS_MAX_AP_DELAY][2] = ps->ap_delay;
#if !USE_FIXED
    const float transient_impact  = 1.5f;
    const float a_smooth          = 0.25f; ///< Smoothing coefficient
#endif /
    const int8_t *k_to_i = is34 ? k_to_i_34 : k_to_i_20;
    int i, k, m, n;
    int n0 = 0, nL = 32;
    const INTFLOAT peak_decay_factor = Q31(0.76592833836465f);

    memset(power, 0, 34 * sizeof(*power));

    if (is34 != ps->is34bands_old) {
        memset(ps->peak_decay_nrg,         0, sizeof(ps->peak_decay_nrg));
        memset(ps->power_smooth,           0, sizeof(ps->power_smooth));
        memset(ps->peak_decay_diff_smooth, 0, sizeof(ps->peak_decay_diff_smooth));
        memset(ps->delay,                  0, sizeof(ps->delay));
        memset(ps->ap_delay,               0, sizeof(ps->ap_delay));
    }

    for (k = 0; k < NR_BANDS[is34]; k++) {
        int i = k_to_i[k];
        ps->dsp.add_squares(power[i], s[k], nL - n0);
    }

    //Transient detection
#if USE_FIXED
    for (i = 0; i < NR_PAR_BANDS[is34]; i++) {
        for (n = n0; n < nL; n++) {
            int decayed_peak;
            int denom;

            decayed_peak = (int)(((int64_t)peak_decay_factor * \
                                           peak_decay_nrg[i] + 0x40000000) >> 31);
            peak_decay_nrg[i] = FFMAX(decayed_peak, power[i][n]);
            power_smooth[i] += (power[i][n] - power_smooth[i] + 2) >> 2;
            peak_decay_diff_smooth[i] += (peak_decay_nrg[i] - power[i][n] - \
                                          peak_decay_diff_smooth[i] + 2) >> 2;
            denom = peak_decay_diff_smooth[i] + (peak_decay_diff_smooth[i] >> 1);
            if (denom > power_smooth[i]) {
              int p = power_smooth[i];
              while (denom < 0x40000000) {
                denom <<= 1;
                p <<= 1;
              }
              transient_gain[i][n] = p / (denom >> 16);
            }
            else {
              transient_gain[i][n] = 1 << 16;
            }
        }
    }
#else
    for (i = 0; i < NR_PAR_BANDS[is34]; i++) {
        for (n = n0; n < nL; n++) {
            float decayed_peak = peak_decay_factor * peak_decay_nrg[i];
            float denom;
            peak_decay_nrg[i] = FFMAX(decayed_peak, power[i][n]);
            power_smooth[i] += a_smooth * (power[i][n] - power_smooth[i]);
            peak_decay_diff_smooth[i] += a_smooth * (peak_decay_nrg[i] - power[i][n] - peak_decay_diff_smooth[i]);
            denom = transient_impact * peak_decay_diff_smooth[i];
            transient_gain[i][n]   = (denom > power_smooth[i]) ?
                                         power_smooth[i] / denom : 1.0f;
        }
    }

#endif /
    //Decorrelation and transient reduction
    //                         PS_AP_LINKS - 1
    //                               -----
    //                                | |  Q_fract_allpass[k][m]*z^-link_delay[m] - a[m]*g_decay_slope[k]
    //H[k][z] = z^-2 * phi_fract[k] * | | ----------------------------------------------------------------
    //                                | | 1 - a[m]*g_decay_slope[k]*Q_fract_allpass[k][m]*z^-link_delay[m]
    //                               m = 0
    //d[k][z] (out) = transient_gain_mapped[k][z] * H[k][z] * s[k][z]
    for (k = 0; k < NR_ALLPASS_BANDS[is34]; k++) {
        int b = k_to_i[k];
#if USE_FIXED
        int g_decay_slope;

        if (k - DECAY_CUTOFF[is34] <= 0) {
          g_decay_slope = 1 << 30;
        }
        else if (k - DECAY_CUTOFF[is34] >= 20) {
          g_decay_slope = 0;
        }
        else {
          g_decay_slope = (1 << 30) - DECAY_SLOPE * (k - DECAY_CUTOFF[is34]);
        }
#else
        float g_decay_slope = 1.f - DECAY_SLOPE * (k - DECAY_CUTOFF[is34]);
        g_decay_slope = av_clipf(g_decay_slope, 0.f, 1.f);
#endif /
        memcpy(delay[k], delay[k]+nL, PS_MAX_DELAY*sizeof(delay[k][0]));
        memcpy(delay[k]+PS_MAX_DELAY, s[k], numQMFSlots*sizeof(delay[k][0]));
        for (m = 0; m < PS_AP_LINKS; m++) {
            memcpy(ap_delay[k][m],   ap_delay[k][m]+numQMFSlots,           5*sizeof(ap_delay[k][m][0]));
        }
        ps->dsp.decorrelate(out[k], delay[k] + PS_MAX_DELAY - 2, ap_delay[k],
                            phi_fract[is34][k],
                            (const INTFLOAT (*)[2]) Q_fract_allpass[is34][k],
                            transient_gain[b], g_decay_slope, nL - n0);
    }
    for (; k < SHORT_DELAY_BAND[is34]; k++) {
        int i = k_to_i[k];
        memcpy(delay[k], delay[k]+nL, PS_MAX_DELAY*sizeof(delay[k][0]));
        memcpy(delay[k]+PS_MAX_DELAY, s[k], numQMFSlots*sizeof(delay[k][0]));
        //H = delay 14
        ps->dsp.mul_pair_single(out[k], delay[k] + PS_MAX_DELAY - 14,
                                transient_gain[i], nL - n0);
    }
    for (; k < NR_BANDS[is34]; k++) {
        int i = k_to_i[k];
        memcpy(delay[k], delay[k]+nL, PS_MAX_DELAY*sizeof(delay[k][0]));
        memcpy(delay[k]+PS_MAX_DELAY, s[k], numQMFSlots*sizeof(delay[k][0]));
        //H = delay 1
        ps->dsp.mul_pair_single(out[k], delay[k] + PS_MAX_DELAY - 1,
                                transient_gain[i], nL - n0);
    }
}
