void ff_snow_horizontal_compose97i_mmx(DWTELEM *b, int width){
    const int w2= (width+1)>>1;
    DWTELEM temp[width >> 1];
    const int w_l= (width>>1);
    const int w_r= w2 - 1;
    int i;

    { // Lift 0
        DWTELEM * const ref = b + w2 - 1;

        i = 1;
        b[0] = b[0] - ((W_DM * 2 * ref[1]+W_DO)>>W_DS);
        asm volatile(
            "pcmpeqd    %%mm7, %%mm7         \n\t"
            "pslld        $31, %%mm7         \n\t"
            "psrld        $29, %%mm7         \n\t"
           ::);
        for(; i<w_l-3; i+=4){
            asm volatile(
                "movq     (%1), %%mm2        \n\t"
                "movq    8(%1), %%mm6        \n\t"
                "paddd   4(%1), %%mm2        \n\t"
                "paddd  12(%1), %%mm6        \n\t"
                "movq    %%mm2, %%mm0        \n\t"
                "movq    %%mm6, %%mm4        \n\t"
                "paddd   %%mm2, %%mm2        \n\t"
                "paddd   %%mm6, %%mm6        \n\t"
                "paddd   %%mm0, %%mm2        \n\t"
                "paddd   %%mm4, %%mm6        \n\t"
                "paddd   %%mm7, %%mm2        \n\t"
                "paddd   %%mm7, %%mm6        \n\t"
                "psrad      $3, %%mm2        \n\t"
                "psrad      $3, %%mm6        \n\t"
                "movq     (%0), %%mm0        \n\t"
                "movq    8(%0), %%mm4        \n\t"
                "psubd   %%mm2, %%mm0        \n\t"
                "psubd   %%mm6, %%mm4        \n\t"
                "movq    %%mm0, (%0)         \n\t"
                "movq    %%mm4, 8(%0)        \n\t"
                :: "r"(&b[i]), "r"(&ref[i])
                 : "memory"
               );
        }
        snow_horizontal_compose_lift_lead_out(i, b, b, ref, width, w_l, 0, W_DM, W_DO, W_DS);
    }

    { // Lift 1
        DWTELEM * const dst = b+w2;

        i = 0;
        for(; i<w_r-3; i+=4){
            asm volatile(
                "movq     (%1), %%mm2        \n\t"
                "movq    8(%1), %%mm6        \n\t"
                "paddd   4(%1), %%mm2        \n\t"
                "paddd  12(%1), %%mm6        \n\t"
                "movq     (%0), %%mm0        \n\t"
                "movq    8(%0), %%mm4        \n\t"
                "psubd   %%mm2, %%mm0        \n\t"
                "psubd   %%mm6, %%mm4        \n\t"
                "movq    %%mm0, (%0)         \n\t"
                "movq    %%mm4, 8(%0)        \n\t"
                :: "r"(&dst[i]), "r"(&b[i])
                 : "memory"
               );
        }
        snow_horizontal_compose_lift_lead_out(i, dst, dst, b, width, w_r, 1, W_CM, W_CO, W_CS);
    }

    { // Lift 2
        DWTELEM * const ref = b+w2 - 1;

        i = 1;
        b[0] = b[0] + (((2 * ref[1] + W_BO) + 4 * b[0]) >> W_BS);
        asm volatile(
            "pslld          $1, %%mm7        \n\t"
           ::);
        for(; i<w_l-3; i+=4){
            asm volatile(
                "movq     (%1), %%mm0        \n\t"
                "movq    8(%1), %%mm4        \n\t"
                "paddd   4(%1), %%mm0        \n\t"
                "paddd  12(%1), %%mm4        \n\t"
                "paddd   %%mm7, %%mm0        \n\t"
                "paddd   %%mm7, %%mm4        \n\t"
                "psrad      $2, %%mm0        \n\t"
                "psrad      $2, %%mm4        \n\t"
                "movq     (%0), %%mm1        \n\t"
                "movq    8(%0), %%mm5        \n\t"
                "paddd   %%mm1, %%mm0        \n\t"
                "paddd   %%mm5, %%mm4        \n\t"
                "psrad      $2, %%mm0        \n\t"
                "psrad      $2, %%mm4        \n\t"
                "paddd   %%mm1, %%mm0        \n\t"
                "paddd   %%mm5, %%mm4        \n\t"
                "movq    %%mm0, (%0)         \n\t"
                "movq    %%mm4, 8(%0)        \n\t"
                :: "r"(&b[i]), "r"(&ref[i])
                 : "memory"
               );
        }
        snow_horizontal_compose_liftS_lead_out(i, b, b, ref, width, w_l);
    }

    { // Lift 3
        DWTELEM * const src = b+w2;
        i = 0;

        for(; i<w_r-3; i+=4){
            asm volatile(
                "movq    4(%1), %%mm2        \n\t"
                "movq   12(%1), %%mm6        \n\t"
                "paddd    (%1), %%mm2        \n\t"
                "paddd   8(%1), %%mm6        \n\t"
                "movq     (%0), %%mm0        \n\t"
                "movq    8(%0), %%mm4        \n\t"
                "paddd   %%mm2, %%mm0        \n\t"
                "paddd   %%mm6, %%mm4        \n\t"
                "psrad      $1, %%mm2        \n\t"
                "psrad      $1, %%mm6        \n\t"
                "paddd   %%mm0, %%mm2        \n\t"
                "paddd   %%mm4, %%mm6        \n\t"
                "movq    %%mm2, (%2)         \n\t"
                "movq    %%mm6, 8(%2)        \n\t"
                :: "r"(&src[i]), "r"(&b[i]), "r"(&temp[i])
                 : "memory"
               );
        }
        snow_horizontal_compose_lift_lead_out(i, temp, src, b, width, w_r, 1, -W_AM, W_AO+1, W_AS);
    }

    {
        snow_interleave_line_header(&i, width, b, temp);

        for (; (i & 0xE) != 0xE; i-=2){
            b[i+1] = temp[i>>1];
            b[i] = b[i>>1];
        }
        for (i-=14; i>=0; i-=16){
            asm volatile(
                "movq        (%1), %%mm0       \n\t"
                "movq       8(%1), %%mm2       \n\t"
                "movq      16(%1), %%mm4       \n\t"
                "movq      24(%1), %%mm6       \n\t"
                "movq        (%1), %%mm1       \n\t"
                "movq       8(%1), %%mm3       \n\t"
                "movq      16(%1), %%mm5       \n\t"
                "movq      24(%1), %%mm7       \n\t"
                "punpckldq   (%2), %%mm0       \n\t"
                "punpckldq  8(%2), %%mm2       \n\t"
                "punpckldq 16(%2), %%mm4       \n\t"
                "punpckldq 24(%2), %%mm6       \n\t"
                "movq       %%mm0, (%0)        \n\t"
                "movq       %%mm2, 16(%0)      \n\t"
                "movq       %%mm4, 32(%0)      \n\t"
                "movq       %%mm6, 48(%0)      \n\t"
                "punpckhdq   (%2), %%mm1       \n\t"
                "punpckhdq  8(%2), %%mm3       \n\t"
                "punpckhdq 16(%2), %%mm5       \n\t"
                "punpckhdq 24(%2), %%mm7       \n\t"
                "movq       %%mm1, 8(%0)       \n\t"
                "movq       %%mm3, 24(%0)      \n\t"
                "movq       %%mm5, 40(%0)      \n\t"
                "movq       %%mm7, 56(%0)      \n\t"
                :: "r"(&b[i]), "r"(&b[i>>1]), "r"(&temp[i>>1])
                 : "memory"
               );
        }
    }
}
