void ff_snow_horizontal_compose97i_sse2(IDWTELEM *b, int width){
    const int w2= (width+1)>>1;
    // SSE2 code runs faster with pointers aligned on a 32-byte boundary.
    IDWTELEM temp_buf[(width>>1) + 4];
    IDWTELEM * const temp = temp_buf + 4 - (((int)temp_buf & 0xF) >> 2);
    const int w_l= (width>>1);
    const int w_r= w2 - 1;
    int i;

    { // Lift 0
        IDWTELEM * const ref = b + w2 - 1;
        IDWTELEM b_0 = b[0]; //By allowing the first entry in b[0] to be calculated twice
        // (the first time erroneously), we allow the SSE2 code to run an extra pass.
        // The savings in code and time are well worth having to store this value and
        // calculate b[0] correctly afterwards.

        i = 0;
        asm volatile(
            "pcmpeqd   %%xmm7, %%xmm7         \n\t"
            "psllw        $15, %%xmm7         \n\t"
            "psrlw        $14, %%xmm7         \n\t"
        ::);
        for(; i<w_l-15; i+=16){
            asm volatile(
                "movdqu   (%1), %%xmm1        \n\t"
                "movdqu 16(%1), %%xmm5        \n\t"
                "movdqu  2(%1), %%xmm2        \n\t"
                "movdqu 18(%1), %%xmm6        \n\t"
                "paddw  %%xmm1, %%xmm2        \n\t"
                "paddw  %%xmm5, %%xmm6        \n\t"
                "movdqa %%xmm2, %%xmm0        \n\t"
                "movdqa %%xmm6, %%xmm4        \n\t"
                "psraw      $1, %%xmm0        \n\t"
                "psraw      $1, %%xmm4        \n\t"
                "paddw  %%xmm0, %%xmm2        \n\t"
                "paddw  %%xmm4, %%xmm6        \n\t"
                "paddw  %%xmm7, %%xmm2        \n\t"
                "paddw  %%xmm7, %%xmm6        \n\t"
                "psraw      $2, %%xmm2        \n\t"
                "psraw      $2, %%xmm6        \n\t"
                "movdqa   (%0), %%xmm0        \n\t"
                "movdqa 16(%0), %%xmm4        \n\t"
                "psubw  %%xmm2, %%xmm0        \n\t"
                "psubw  %%xmm6, %%xmm4        \n\t"
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
        IDWTELEM * const dst = b+w2;

        i = 0;
        for(; (((long)&dst[i]) & 0x1F) && i<w_r; i++){
            dst[i] = dst[i] - (b[i] + b[i + 1]);
        }
        for(; i<w_r-15; i+=16){
            asm volatile(
                "movdqu   (%1), %%xmm1        \n\t"
                "movdqu 16(%1), %%xmm5        \n\t"
                "movdqu  2(%1), %%xmm2        \n\t"
                "movdqu 18(%1), %%xmm6        \n\t"
                "paddw  %%xmm1, %%xmm2        \n\t"
                "paddw  %%xmm5, %%xmm6        \n\t"
                "movdqa   (%0), %%xmm0        \n\t"
                "movdqa 16(%0), %%xmm4        \n\t"
                "psubw  %%xmm2, %%xmm0        \n\t"
                "psubw  %%xmm6, %%xmm4        \n\t"
                "movdqa %%xmm0, (%0)          \n\t"
                "movdqa %%xmm4, 16(%0)        \n\t"
                :: "r"(&dst[i]), "r"(&b[i])
                : "memory"
            );
        }
        snow_horizontal_compose_lift_lead_out(i, dst, dst, b, width, w_r, 1, W_CM, W_CO, W_CS);
    }

    { // Lift 2
        IDWTELEM * const ref = b+w2 - 1;
        IDWTELEM b_0 = b[0];

        i = 0;
        asm volatile(
            "psllw          $2, %%xmm7        \n\t"
        ::);
        for(; i<w_l-15; i+=16){
            asm volatile(
                "movdqu   (%1), %%xmm1        \n\t"
                "movdqu 16(%1), %%xmm5        \n\t"
                "movdqu  2(%1), %%xmm0        \n\t"
                "movdqu 18(%1), %%xmm4        \n\t" //FIXME try aligned reads and shifts
                "paddw  %%xmm1, %%xmm0        \n\t"
                "paddw  %%xmm5, %%xmm4        \n\t"
                "paddw  %%xmm7, %%xmm0        \n\t"
                "paddw  %%xmm7, %%xmm4        \n\t"
                "movdqa   (%0), %%xmm1        \n\t"
                "movdqa 16(%0), %%xmm5        \n\t"
                "psraw      $2, %%xmm0        \n\t"
                "psraw      $2, %%xmm4        \n\t"
                "paddw  %%xmm1, %%xmm0        \n\t"
                "paddw  %%xmm5, %%xmm4        \n\t"
                "psraw      $2, %%xmm0        \n\t"
                "psraw      $2, %%xmm4        \n\t"
                "paddw  %%xmm1, %%xmm0        \n\t"
                "paddw  %%xmm5, %%xmm4        \n\t"
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
        IDWTELEM * const src = b+w2;

        i = 0;
        for(; (((long)&temp[i]) & 0x1F) && i<w_r; i++){
            temp[i] = src[i] - ((-W_AM*(b[i] + b[i+1]))>>W_AS);
        }
        for(; i<w_r-7; i+=8){
            asm volatile(
                "movdqu  2(%1), %%xmm2        \n\t"
                "movdqu 18(%1), %%xmm6        \n\t"
                "paddw    (%1), %%xmm2        \n\t"
                "paddw  16(%1), %%xmm6        \n\t"
                "movdqu   (%0), %%xmm0        \n\t"
                "movdqu 16(%0), %%xmm4        \n\t"
                "paddw  %%xmm2, %%xmm0        \n\t"
                "paddw  %%xmm6, %%xmm4        \n\t"
                "psraw      $1, %%xmm2        \n\t"
                "psraw      $1, %%xmm6        \n\t"
                "paddw  %%xmm0, %%xmm2        \n\t"
                "paddw  %%xmm4, %%xmm6        \n\t"
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

        for (; (i & 0x3E) != 0x3E; i-=2){
            b[i+1] = temp[i>>1];
            b[i] = b[i>>1];
        }
        for (i-=62; i>=0; i-=64){
            asm volatile(
                "movdqa      (%1), %%xmm0       \n\t"
                "movdqa    16(%1), %%xmm2       \n\t"
                "movdqa    32(%1), %%xmm4       \n\t"
                "movdqa    48(%1), %%xmm6       \n\t"
                "movdqa      (%1), %%xmm1       \n\t"
                "movdqa    16(%1), %%xmm3       \n\t"
                "movdqa    32(%1), %%xmm5       \n\t"
                "movdqa    48(%1), %%xmm7       \n\t"
                "punpcklwd   (%2), %%xmm0       \n\t"
                "punpcklwd 16(%2), %%xmm2       \n\t"
                "punpcklwd 32(%2), %%xmm4       \n\t"
                "punpcklwd 48(%2), %%xmm6       \n\t"
                "movdqa    %%xmm0, (%0)         \n\t"
                "movdqa    %%xmm2, 32(%0)       \n\t"
                "movdqa    %%xmm4, 64(%0)       \n\t"
                "movdqa    %%xmm6, 96(%0)       \n\t"
                "punpckhwd   (%2), %%xmm1       \n\t"
                "punpckhwd 16(%2), %%xmm3       \n\t"
                "punpckhwd 32(%2), %%xmm5       \n\t"
                "punpckhwd 48(%2), %%xmm7       \n\t"
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
