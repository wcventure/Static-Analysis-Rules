static void process_tns_coeffs(TemporalNoiseShaping *tns, double *coef_raw,
                               int *order_p, int w, int filt)
{
    int i, j, order = *order_p;
    int *idx = tns->coef_idx[w][filt];
    float *lpc = tns->coef[w][filt];
    float temp[TNS_MAX_ORDER] = {0.0f}, out[TNS_MAX_ORDER] = {0.0f};

    if (!order)
        return;

    /
    for (i = 0; i < order; i++) {
        idx[i] = quant_array_idx(coef_raw[i], tns_tmp2_map_0_4, 16);
        lpc[i] = tns_tmp2_map_0_4[idx[i]];
    }

    /
    for (i = order-1; i > -1; i--) {
        lpc[i] = (fabs(lpc[i]) > 0.1f) ? lpc[i] : 0.0f;
        if (lpc[i] != 0.0 ) {
            order = i;
            break;
        }
    }

    /
    out[0] = 1.0f;
    for (i = 1; i <= order; i++) {
        for (j = 1; j < i; j++) {
            temp[j] = out[j] + lpc[i]*out[i-j];
        }
        for (j = 1; j <= i; j++) {
            out[j] = temp[j];
        }
        out[i] = lpc[i-1];
    }
    *order_p = order;
    memcpy(lpc, out, TNS_MAX_ORDER*sizeof(float));
}
