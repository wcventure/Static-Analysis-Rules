static void flac_lpc_16_c(int32_t *decoded, const int coeffs[32],
                          int pred_order, int qlevel, int len)
{
    int i, j;

    for (i = pred_order; i < len - 1; i += 2, decoded += 2) {
        SUINT c = coeffs[0];
        SUINT d = decoded[0];
        int s0 = 0, s1 = 0;
        for (j = 1; j < pred_order; j++) {
            s0 += c*d;
            d = decoded[j];
            s1 += c*d;
            c = coeffs[j];
        }
        s0 += c*d;
        d = decoded[j] += s0 >> qlevel;
        s1 += c*d;
        decoded[j + 1] += s1 >> qlevel;
    }
    if (i < len) {
        int sum = 0;
        for (j = 0; j < pred_order; j++)
            sum += coeffs[j] * (SUINT)decoded[j];
        decoded[j] += sum >> qlevel;
    }
}
