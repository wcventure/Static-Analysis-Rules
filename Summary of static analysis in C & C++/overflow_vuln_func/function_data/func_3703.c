static void mdct512(AC3MDCTContext *mdct, int32_t *out, int16_t *in)
{
    int i, re, im, n, n2, n4;
    int16_t *rot = mdct->rot_tmp;
    IComplex *x  = mdct->cplx_tmp;

    n  = 1 << mdct->nbits;
    n2 = n >> 1;
    n4 = n >> 2;

    /
    for (i = 0; i <n4; i++)
        rot[i] = -in[i + 3*n4];
    memcpy(&rot[n4], &in[0], 3*n4*sizeof(*in));

    /
    for (i = 0; i < n4; i++) {
        re =  ((int)rot[   2*i] - (int)rot[ n-1-2*i]) >> 1;
        im = -((int)rot[n2+2*i] - (int)rot[n2-1-2*i]) >> 1;
        CMUL(x[i].re, x[i].im, re, im, -mdct->xcos1[i], mdct->xsin1[i]);
    }

    fft(mdct, x, mdct->nbits - 2);

    /
    for (i = 0; i < n4; i++) {
        re = x[i].re;
        im = x[i].im;
        CMUL(out[n2-1-2*i], out[2*i], re, im, mdct->xsin1[i], mdct->xcos1[i]);
    }
}
