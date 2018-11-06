static void fft(AC3MDCTContext *mdct, IComplex *z, int ln)
{
    int j, l, np, np2;
    int nblocks, nloops;
    register IComplex *p,*q;
    int tmp_re, tmp_im;

    np = 1 << ln;

    /
    for (j = 0; j < np; j++) {
        int k = av_reverse[j] >> (8 - ln);
        if (k < j)
            FFSWAP(IComplex, z[k], z[j]);
    }

    /

    p = &z[0];
    j = np >> 1;
    do {
        BF(p[0].re, p[0].im, p[1].re, p[1].im,
           p[0].re, p[0].im, p[1].re, p[1].im);
        p += 2;
    } while (--j);

    /

    p = &z[0];
    j = np >> 2;
    do {
        BF(p[0].re, p[0].im, p[2].re,  p[2].im,
           p[0].re, p[0].im, p[2].re,  p[2].im);
        BF(p[1].re, p[1].im, p[3].re,  p[3].im,
           p[1].re, p[1].im, p[3].im, -p[3].re);
        p+=4;
    } while (--j);

    /

    nblocks = np >> 3;
    nloops  =  1 << 2;
    np2     = np >> 1;
    do {
        p = z;
        q = z + nloops;
        for (j = 0; j < nblocks; j++) {
            BF(p->re, p->im, q->re, q->im,
               p->re, p->im, q->re, q->im);
            p++;
            q++;
            for(l = nblocks; l < np2; l += nblocks) {
                CMUL(tmp_re, tmp_im, mdct->costab[l], -mdct->sintab[l], q->re, q->im);
                BF(p->re, p->im, q->re,  q->im,
                   p->re, p->im, tmp_re, tmp_im);
                p++;
                q++;
            }
            p += nloops;
            q += nloops;
        }
        nblocks = nblocks >> 1;
        nloops  = nloops  << 1;
    } while (nblocks);
}
