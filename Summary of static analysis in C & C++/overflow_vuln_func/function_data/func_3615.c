void ff_snow_vertical_compose97i_sse2(IDWTELEM *b0, IDWTELEM *b1, IDWTELEM *b2, IDWTELEM *b3, IDWTELEM *b4, IDWTELEM *b5, int width){
    long i = width;

    while(i & 0x1F)
    {
        i--;
        b4[i] -= (W_DM*(b3[i] + b5[i])+W_DO)>>W_DS;
        b3[i] -= (W_CM*(b2[i] + b4[i])+W_CO)>>W_CS;
        b2[i] += (W_BM*(b1[i] + b3[i])+4*b2[i]+W_BO)>>W_BS;
        b1[i] += (W_AM*(b0[i] + b2[i])+W_AO)>>W_AS;
    }
    i+=i;

         asm volatile (
        "jmp 2f                                      \n\t"
        "1:                                          \n\t"

        snow_vertical_compose_sse2_load("%4","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_add("%6","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_move("xmm0","xmm2","xmm4","xmm6","xmm1","xmm3","xmm5","xmm7")
        snow_vertical_compose_sra("1","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_r2r_add("xmm1","xmm3","xmm5","xmm7","xmm0","xmm2","xmm4","xmm6")

        "pcmpeqd %%xmm1, %%xmm1                      \n\t"
        "psllw $15, %%xmm1                           \n\t"
        "psrlw $14, %%xmm1                           \n\t"

        snow_vertical_compose_r2r_add("xmm1","xmm1","xmm1","xmm1","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sra("2","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_load("%5","xmm1","xmm3","xmm5","xmm7")
        snow_vertical_compose_r2r_sub("xmm0","xmm2","xmm4","xmm6","xmm1","xmm3","xmm5","xmm7")
        snow_vertical_compose_sse2_store("%5","xmm1","xmm3","xmm5","xmm7")
        snow_vertical_compose_sse2_load("%4","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_add("%3","xmm1","xmm3","xmm5","xmm7")
        snow_vertical_compose_r2r_sub("xmm1","xmm3","xmm5","xmm7","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_store("%4","xmm0","xmm2","xmm4","xmm6")

        "pcmpeqw %%xmm7, %%xmm7                      \n\t"
        "pcmpeqw %%xmm5, %%xmm5                      \n\t"
        "psllw $15, %%xmm7                           \n\t"
        "psrlw $13, %%xmm5                           \n\t"
        "paddw %%xmm7, %%xmm5                        \n\t"
        snow_vertical_compose_r2r_add("xmm5","xmm5","xmm5","xmm5","xmm0","xmm2","xmm4","xmm6")
        "movq   (%2,%%"REG_d"), %%xmm1        \n\t"
        "movq  8(%2,%%"REG_d"), %%xmm3        \n\t"
        "paddw %%xmm7, %%xmm1                        \n\t"
        "paddw %%xmm7, %%xmm3                        \n\t"
        "pavgw %%xmm1, %%xmm0                        \n\t"
        "pavgw %%xmm3, %%xmm2                        \n\t"
        "movq 16(%2,%%"REG_d"), %%xmm1        \n\t"
        "movq 24(%2,%%"REG_d"), %%xmm3        \n\t"
        "paddw %%xmm7, %%xmm1                        \n\t"
        "paddw %%xmm7, %%xmm3                        \n\t"
        "pavgw %%xmm1, %%xmm4                        \n\t"
        "pavgw %%xmm3, %%xmm6                        \n\t"
        snow_vertical_compose_r2r_sub("xmm7","xmm7","xmm7","xmm7","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sra("1","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_add("%3","xmm0","xmm2","xmm4","xmm6")

        snow_vertical_compose_sra("2","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_add("%3","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_store("%3","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_add("%1","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_move("xmm0","xmm2","xmm4","xmm6","xmm1","xmm3","xmm5","xmm7")
        snow_vertical_compose_sra("1","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_r2r_add("xmm1","xmm3","xmm5","xmm7","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_add("%2","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_store("%2","xmm0","xmm2","xmm4","xmm6")

        "2:                                          \n\t"
        "sub $64, %%"REG_d"                          \n\t"
        "jge 1b                                      \n\t"
        :"+d"(i)
        :"r"(b0),"r"(b1),"r"(b2),"r"(b3),"r"(b4),"r"(b5));
}
