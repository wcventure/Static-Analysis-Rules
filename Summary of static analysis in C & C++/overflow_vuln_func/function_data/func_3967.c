static void decode_lpc(int32_t *coeffs, int mode, int length)
{
    int i;

    if (length < 2)
        return;

    if (mode == 1) {
        unsigned a1 = *coeffs++;
        for (i = 0; i < length - 1 >> 1; i++) {
            *coeffs   += a1;
            coeffs[1] += *coeffs;
            a1         = coeffs[1];
            coeffs    += 2;
        }
        if (length - 1 & 1)
            *coeffs += a1;
    } else if (mode == 2) {
        unsigned a1    = coeffs[1];
        unsigned a2    = a1 + *coeffs;
        coeffs[1] = a2;
        if (length > 2) {
            coeffs += 2;
            for (i = 0; i < length - 2 >> 1; i++) {
                unsigned a3    = *coeffs + a1;
                unsigned a4    = a3 + a2;
                *coeffs   = a4;
                a1        = coeffs[1] + a3;
                a2        = a1 + a4;
                coeffs[1] = a2;
                coeffs   += 2;
            }
            if (length & 1)
                *coeffs += a1 + a2;
        }
    } else if (mode == 3) {
        unsigned a1    = coeffs[1];
        unsigned a2    = a1 + *coeffs;
        coeffs[1] = a2;
        if (length > 2) {
            unsigned a3  = coeffs[2];
            unsigned a4  = a3 + a1;
            unsigned a5  = a4 + a2;
            coeffs[2] = a5;
            coeffs += 3;
            for (i = 0; i < length - 3; i++) {
                a3     += *coeffs;
                a4     += a3;
                a5     += a4;
                *coeffs = a5;
                coeffs++;
            }
        }
    }
}
