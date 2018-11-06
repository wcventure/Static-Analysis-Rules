static void quantize_bands(int (*out)[2], const float *in, const float *scaled,
                           int size, float Q34, int is_signed, int maxval)
{
    int i;
    double qc;
    for (i = 0; i < size; i++) {
        qc = scaled[i] * Q34;
        out[i][0] = (int)FFMIN((int)qc,            maxval);
        out[i][1] = (int)FFMIN((int)(qc + 0.4054), maxval);
        if (is_signed && in[i] < 0.0f) {
            out[i][0] = -out[i][0];
            out[i][1] = -out[i][1];
        }
    }
}
