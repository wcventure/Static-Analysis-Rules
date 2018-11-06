void ff_imdct_half_3dn2(FFTContext *s, FFTSample *output, const FFTSample *input)
{
    x86_reg j, k;
    long n = s->mdct_size;
    long n2 = n >> 1;
    long n4 = n >> 2;
    long n8 = n >> 3;
    const uint16_t *revtab = s->revtab;
    const FFTSample *tcos = s->tcos;
    const FFTSample *tsin = s->tsin;
    const FFTSample *in1, *in2;
    FFTComplex *z = (FFTComplex *)output;

    /
    in1 = input;
    in2 = input + n2 - 1;
#ifdef EMULATE_3DNOWEXT
    __asm__ volatile("movd %0, %%mm7" ::"r"(1<<31));
#endif
    for(k = 0; k < n4; k++) {
        // FIXME a single block is faster, but gcc 2.95 and 3.4.x on 32bit can't compile it
        __asm__ volatile(
            "movd         %0, %%mm0 \n"
            "movd         %2, %%mm1 \n"
            "punpckldq    %1, %%mm0 \n"
            "punpckldq    %3, %%mm1 \n"
            "movq      %%mm0, %%mm2 \n"
            PSWAPD(    %%mm1, %%mm3 )
            "pfmul     %%mm1, %%mm0 \n"
            "pfmul     %%mm3, %%mm2 \n"
#ifdef EMULATE_3DNOWEXT
            "movq      %%mm0, %%mm1 \n"
            "punpckhdq %%mm2, %%mm0 \n"
            "punpckldq %%mm2, %%mm1 \n"
            "pxor      %%mm7, %%mm0 \n"
            "pfadd     %%mm1, %%mm0 \n"
#else
            "pfpnacc   %%mm2, %%mm0 \n"
#endif
            ::"m"(in2[-2*k]), "m"(in1[2*k]),
              "m"(tcos[k]), "m"(tsin[k])
        );
        __asm__ volatile(
            "movq    %%mm0, %0    \n\t"
            :"=m"(z[revtab[k]])
        );
    }

    ff_fft_dispatch_3dn2(z, s->nbits);

#define CMUL(j,mm0,mm1)\
        "movq  (%2,"#j",2), %%mm6 \n"\
        "movq 8(%2,"#j",2), "#mm0"\n"\
        "movq        %%mm6, "#mm1"\n"\
        "movq        "#mm0",%%mm7 \n"\
        "pfmul   (%3,"#j"), %%mm6 \n"\
        "pfmul   (%4,"#j"), "#mm0"\n"\
        "pfmul   (%4,"#j"), "#mm1"\n"\
        "pfmul   (%3,"#j"), %%mm7 \n"\
        "pfsub       %%mm6, "#mm0"\n"\
        "pfadd       %%mm7, "#mm1"\n"

    /
    j = -n2;
    k = n2-8;
    __asm__ volatile(
        "1: \n"
        CMUL(%0, %%mm0, %%mm1)
        CMUL(%1, %%mm2, %%mm3)
        "movd   %%mm0,  (%2,%0,2) \n"
        "movd   %%mm1,12(%2,%1,2) \n"
        "movd   %%mm2,  (%2,%1,2) \n"
        "movd   %%mm3,12(%2,%0,2) \n"
        "psrlq  $32,   %%mm0 \n"
        "psrlq  $32,   %%mm1 \n"
        "psrlq  $32,   %%mm2 \n"
        "psrlq  $32,   %%mm3 \n"
        "movd   %%mm0, 8(%2,%0,2) \n"
        "movd   %%mm1, 4(%2,%1,2) \n"
        "movd   %%mm2, 8(%2,%1,2) \n"
        "movd   %%mm3, 4(%2,%0,2) \n"
        "sub $8, %1 \n"
        "add $8, %0 \n"
        "jl 1b \n"
        :"+r"(j), "+r"(k)
        :"r"(z+n8), "r"(tcos+n8), "r"(tsin+n8)
        :"memory"
    );
    __asm__ volatile("femms");
}
