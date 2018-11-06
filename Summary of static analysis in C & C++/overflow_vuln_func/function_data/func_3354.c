static void ps_decorrelate_c(INTFLOAT (*out)[2], INTFLOAT (*delay)[2],
                             INTFLOAT (*ap_delay)[PS_QMF_TIME_SLOTS + PS_MAX_AP_DELAY][2],
                             const INTFLOAT phi_fract[2], const INTFLOAT (*Q_fract)[2],
                             const INTFLOAT *transient_gain,
                             INTFLOAT g_decay_slope,
                             int len)
{
    static const INTFLOAT a[] = { Q31(0.65143905753106f),
                               Q31(0.56471812200776f),
                               Q31(0.48954165955695f) };
    INTFLOAT ag[PS_AP_LINKS];
    int m, n;

    for (m = 0; m < PS_AP_LINKS; m++)
        ag[m] = AAC_MUL30(a[m], g_decay_slope);

    for (n = 0; n < len; n++) {
        INTFLOAT in_re = AAC_MSUB30(delay[n][0], phi_fract[0], delay[n][1], phi_fract[1]);
        INTFLOAT in_im = AAC_MADD30(delay[n][0], phi_fract[1], delay[n][1], phi_fract[0]);
        for (m = 0; m < PS_AP_LINKS; m++) {
            INTFLOAT a_re                = AAC_MUL31(ag[m], in_re);
            INTFLOAT a_im                = AAC_MUL31(ag[m], in_im);
            INTFLOAT link_delay_re       = ap_delay[m][n+2-m][0];
            INTFLOAT link_delay_im       = ap_delay[m][n+2-m][1];
            INTFLOAT fractional_delay_re = Q_fract[m][0];
            INTFLOAT fractional_delay_im = Q_fract[m][1];
            INTFLOAT apd_re = in_re;
            INTFLOAT apd_im = in_im;
            in_re = AAC_MSUB30(link_delay_re, fractional_delay_re,
                    link_delay_im, fractional_delay_im);
            in_re -= a_re;
            in_im = AAC_MADD30(link_delay_re, fractional_delay_im,
                    link_delay_im, fractional_delay_re);
            in_im -= a_im;
            ap_delay[m][n+5][0] = apd_re + AAC_MUL31(ag[m], in_re);
            ap_delay[m][n+5][1] = apd_im + AAC_MUL31(ag[m], in_im);
        }
        out[n][0] = AAC_MUL16(transient_gain[n], in_re);
        out[n][1] = AAC_MUL16(transient_gain[n], in_im);
    }
}
