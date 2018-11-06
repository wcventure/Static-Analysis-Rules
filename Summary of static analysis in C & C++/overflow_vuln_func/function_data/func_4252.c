void ff_mdct_calc_c(FFTContext *s, FFTSample *out, const FFTSample *input)
{
    int i, j, n, n8, n4, n2, n3;
    FFTDouble re, im;
    const uint16_t *revtab = s->revtab;
    const FFTSample *tcos = s->tcos;
    const FFTSample *tsin = s->tsin;
    FFTComplex *x = (FFTComplex *)out;

    n = 1 << s->mdct_bits;
    n2 = n >> 1;
    n4 = n >> 2;
    n8 = n >> 3;
    n3 = 3 * n4;

    /
    for(i=0;i<n8;i++) {
        re = RSCALE(-input[2*i+n3] - input[n3-1-2*i]);
        im = RSCALE(-input[n4+2*i] + input[n4-1-2*i]);
        j = revtab[i];
        CMUL(x[j].re, x[j].im, re, im, -tcos[i], tsin[i]);

        re = RSCALE( input[2*i]    - input[n2-1-2*i]);
        im = RSCALE(-input[n2+2*i] - input[ n-1-2*i]);
        j = revtab[n8 + i];
        CMUL(x[j].re, x[j].im, re, im, -tcos[n8 + i], tsin[n8 + i]);
    }

    s->fft_calc(s, x);

    /
    for(i=0;i<n8;i++) {
        FFTSample r0, i0, r1, i1;
        CMUL(i1, r0, x[n8-i-1].re, x[n8-i-1].im, -tsin[n8-i-1], -tcos[n8-i-1]);
        CMUL(i0, r1, x[n8+i  ].re, x[n8+i  ].im, -tsin[n8+i  ], -tcos[n8+i  ]);
        x[n8-i-1].re = r0;
        x[n8-i-1].im = i0;
        x[n8+i  ].re = r1;
        x[n8+i  ].im = i1;
    }
}
