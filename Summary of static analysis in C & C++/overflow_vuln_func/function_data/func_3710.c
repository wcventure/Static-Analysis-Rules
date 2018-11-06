void ff_snow_horizontal_compose97i_sse2(DWTELEM *b, int width){
    const int w2= (width+1)>>1;
    // SSE2 code runs faster with pointers aligned on a 32-byte boundary.
    DWTELEM temp_buf[(width>>1) + 4];
    DWTELEM * const temp = temp_buf + 4 - (((int)temp_buf & 0xF) >> 2);
    const int w_l= (width>>1);
    const int w_r= w2 - 1;
    int i;

    { // Lift 0
        DWTELEM * const ref = b + w2 - 1;
        DWTELEM b_0 = b[0]; //By allowing the first entry in b[0] to be calculated twice
        // (the first time erroneously), we allow the SSE2 code to run an extra pass.
        // The savings in code and time are well worth having to store this value and
        // calculate b[0] correctly afterwards.

        i = 0;
        asm volatile(
            "pcmpeqd   %%xmm7, %%xmm7         \n\t"
            "pslld        $31, %%xmm7         \n\t"
            "psrld        $29, %%xmm7         \n\t"
        ::);
        for(; i<w_l-7; i+=8){
            asm volatile(
                "movdqu   (%1), %%xmm1        \n\t"
                "movdqu 16(%1), %%xmm5        \n\t"
                "movdqu  4(%1), %%xmm2        \n\t"
                "movdqu 20(%1), %%xmm6        \n\t"
                "paddd  %%xmm1, %%xmm2        \n\t"
                "paddd  %%xmm5, %%xmm6        \n\t"
                "movdqa %%xmm2, %%xmm0        \n\t"
                "movdqa %%xmm6, %%xmm4        \n\t"
                "paddd  %%xmm2, %%xmm2        \n\t"
                "paddd  %%xmm6, %%xmm6        \n\t"
                "paddd  %%xmm0, %%xmm2        \n\t"
                "paddd  %%xmm4, %%xmm6        \n\t"
                "paddd  %%xmm7, %%xmm2        \n\t"
                "paddd  %%xmm7, %%xmm6        \n\t"
                "psrad      $3, %%xmm2        \n\t"
                "psrad      $3, %%xmm6        \n\t"
                "movdqa   (%0), %%xmm0        \n\t"
                "movdqa 16(%0), %%xmm4        \n\t"
                "psubd  %%xmm2, %%xmm0        \n\t"
                "psubd  %%xmm6, %%xmm4        \n\t"
                "movdqa %%xmm0, (%0)          \n\t"
                "movdqa %%xmm4, 16(%0)        \n\t"
                :: "r"(&b[i]), "r"(&ref[i])
                : "memory"
            );
        }
        snow_horizontal_compose_lift_lead_out(i, b, b, ref, width, w_l, 0, W_DM, W_DO, W_DS);
        b[0] = b_0 - ((W_DM * 2 * ref[1]+W_DO)>>W_DS);
    }

    { // Lift 1
        DWTELEM * const dst = b+w2;

        i = 0;
        for(; (((long)&dst[i]) & 0xF) && i<w_r; i++){
            dst[i] = dst[i] - (b[i] + b[i + 1]);
        }
        for(; i<w_r-7; i+=8){
            asm volatile(
                "movdqu   (%1), %%xmm1        \n\t"
                "movdqu 16(%1), %%xmm5        \n\t"
                "movdqu  4(%1), %%xmm2        \n\t"
                "movdqu 20(%1), %%xmm6        \n\t"
                "paddd  %%xmm1, %%xmm2        \n\t"
                "paddd  %%xmm5, %%xmm6        \n\t"
                "movdqa   (%0), %%xmm0        \n\t"
                "movdqa 16(%0), %%xmm4        \n\t"
                "psubd  %%xmm2, %%xmm0        \n\t"
                "psubd  %%xmm6, %%xmm4        \n\t"
                "movdqa %%xmm0, (%0)          \n\t"
                "movdqa %%xmm4, 16(%0)        \n\t"
                :: "r"(&dst[i]), "r"(&b[i])
                : "memory"
            );
        }
        snow_horizontal_compose_lift_lead_out(i, dst, dst, b, width, w_r, 1, W_CM, W_CO, W_CS);
    }

    { // Lift 2
        DWTELEM * const ref = b+w2 - 1;
        DWTELEM b_0 = b[0];

        i = 0;
        asm volatile(
            "pslld          $1, %%xmm7        \n\t"
        ::);
        for(; i<w_l-7; i+=8){
            asm volatile(
                "movdqu   (%1), %%xmm1        \n\t"
                "movdqu 16(%1), %%xmm5        \n\t"
                "movdqu  4(%1), %%xmm0        \n\t"
                "movdqu 20(%1), %%xmm4        \n\t" //FIXME try aligned reads and shifts
                "paddd  %%xmm1, %%xmm0        \n\t"
                "paddd  %%xmm5, %%xmm4        \n\t"
                "paddd  %%xmm7, %%xmm0        \n\t"
                "paddd  %%xmm7, %%xmm4        \n\t"
                "movdqa   (%0), %%xmm1        \n\t"
                "movdqa 16(%0), %%xmm5        \n\t"
                "psrad      $2, %%xmm0        \n\t"
                "psrad      $2, %%xmm4        \n\t"
                "paddd  %%xmm1, %%xmm0        \n\t"
                "paddd  %%xmm5, %%xmm4        \n\t"
                "psrad      $2, %%xmm0        \n\t"
                "psrad      $2, %%xmm4        \n\t"
                "paddd  %%xmm1, %%xmm0        \n\t"
                "paddd  %%xmm5, %%xmm4        \n\t"
                "movdqa %%xmm0, (%0)          \n\t"
                "movdqa %%xmm4, 16(%0)        \n\t"
                :: "r"(&b[i]), "r"(&ref[i])
                : "memory"
            );
        }
        snow_horizontal_compose_liftS_lead_out(i, b, b, ref, width, w_l);
        b[0] = b_0 + ((2 * ref[1] + W_BO-1 + 4 * b_0) >> W_BS);
    }

    { // Lift 3
        DWTELEM * const src = b+w2;

        i = 0;
        for(; (((long)&temp[i]) & 0xF) && i<w_r; i++){
            temp[i] = src[i] - ((-W_AM*(b[i] + b[i+1]))>>W_AS);
        }
        for(; i<w_r-7; i+=8){
            asm volatile(
                "movdqu  4(%1), %%xmm2        \n\t"
                "movdqu 20(%1), %%xmm6        \n\t"
                "paddd    (%1), %%xmm2        \n\t"
                "paddd  16(%1), %%xmm6        \n\t"
                "movdqu   (%0), %%xmm0        \n\t"
                "movdqu 16(%0), %%xmm4        \n\t"
                "paddd  %%xmm2, %%xmm0        \n\t"
                "paddd  %%xmm6, %%xmm4        \n\t"
                "psrad      $1, %%xmm2        \n\t"
                "psrad      $1, %%xmm6        \n\t"
                "paddd  %%xmm0, %%xmm2        \n\t"
                "paddd  %%xmm4, %%xmm6        \n\t"
                "movdqa %%xmm2, (%2)          \n\t"
                "movdqa %%xmm6, 16(%2)        \n\t"
                :: "r"(&src[i]), "r"(&b[i]), "r"(&temp[i])
                 : "memory"
               );
        }
        snow_horizontal_compose_lift_lead_out(i, temp, src, b, width, w_r, 1, -W_AM, W_AO+1, W_AS);
    }

    {
        snow_interleave_line_header(&i, width, b, temp);

        for (; (i & 0x1E) != 0x1E; i-=2){
            b[i+1] = temp[i>>1];
            b[i] = b[i>>1];
        }
        for (i-=30; i>=0; i-=32){
            asm volatile(
                "movdqa      (%1), %%xmm0       \n\t"
                "movdqa    16(%1), %%xmm2       \n\t"
                "movdqa    32(%1), %%xmm4       \n\t"
                "movdqa    48(%1), %%xmm6       \n\t"
                "movdqa      (%1), %%xmm1       \n\t"
                "movdqa    16(%1), %%xmm3       \n\t"
                "movdqa    32(%1), %%xmm5       \n\t"
                "movdqa    48(%1), %%xmm7       \n\t"
                "punpckldq   (%2), %%xmm0       \n\t"
                "punpckldq 16(%2), %%xmm2       \n\t"
                "punpckldq 32(%2), %%xmm4       \n\t"
                "punpckldq 48(%2), %%xmm6       \n\t"
                "movdqa    %%xmm0, (%0)         \n\t"
                "movdqa    %%xmm2, 32(%0)       \n\t"
                "movdqa    %%xmm4, 64(%0)       \n\t"
                "movdqa    %%xmm6, 96(%0)       \n\t"
                "punpckhdq   (%2), %%xmm1       \n\t"
                "punpckhdq 16(%2), %%xmm3       \n\t"
                "punpckhdq 32(%2), %%xmm5       \n\t"
                "punpckhdq 48(%2), %%xmm7       \n\t"
                "movdqa    %%xmm1, 16(%0)       \n\t"
                "movdqa    %%xmm3, 48(%0)       \n\t"
                "movdqa    %%xmm5, 80(%0)       \n\t"
                "movdqa    %%xmm7, 112(%0)      \n\t"
                :: "r"(&(b)[i]), "r"(&(b)[i>>1]), "r"(&(temp)[i>>1])
                 : "memory"
               );
        }
    }
}
