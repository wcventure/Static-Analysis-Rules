void ff_snow_vertical_compose97i_mmx(IDWTELEM *b0, IDWTELEM *b1, IDWTELEM *b2, IDWTELEM *b3, IDWTELEM *b4, IDWTELEM *b5, int width){
    long i = width;
    while(i & 15)
    {
        i--;
        b4[i] -= (W_DM*(b3[i] + b5[i])+W_DO)>>W_DS;
        b3[i] -= (W_CM*(b2[i] + b4[i])+W_CO)>>W_CS;
        b2[i] += (W_BM*(b1[i] + b3[i])+4*b2[i]+W_BO)>>W_BS;
        b1[i] += (W_AM*(b0[i] + b2[i])+W_AO)>>W_AS;
    }
    i+=i;
    asm volatile(
        "jmp 2f                                      \n\t"
        "1:                                          \n\t"

        snow_vertical_compose_mmx_load("%4","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_add("%6","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_move("mm0","mm2","mm4","mm6","mm1","mm3","mm5","mm7")
        snow_vertical_compose_sra("1","mm0","mm2","mm4","mm6")
        snow_vertical_compose_r2r_add("mm1","mm3","mm5","mm7","mm0","mm2","mm4","mm6")

        "pcmpeqw %%mm1, %%mm1                        \n\t"
        "psllw $15, %%mm1                            \n\t"
        "psrlw $14, %%mm1                            \n\t"

        snow_vertical_compose_r2r_add("mm1","mm1","mm1","mm1","mm0","mm2","mm4","mm6")
        snow_vertical_compose_sra("2","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_load("%5","mm1","mm3","mm5","mm7")
        snow_vertical_compose_r2r_sub("mm0","mm2","mm4","mm6","mm1","mm3","mm5","mm7")
        snow_vertical_compose_mmx_store("%5","mm1","mm3","mm5","mm7")
        snow_vertical_compose_mmx_load("%4","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_add("%3","mm1","mm3","mm5","mm7")
        snow_vertical_compose_r2r_sub("mm1","mm3","mm5","mm7","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_store("%4","mm0","mm2","mm4","mm6")
        "pcmpeqw %%mm7, %%mm7                        \n\t"
        "pcmpeqw %%mm5, %%mm5                        \n\t"
        "psllw $15, %%mm7                            \n\t"
        "psrlw $13, %%mm5                            \n\t"
        "paddw %%mm7, %%mm5                          \n\t"
        snow_vertical_compose_r2r_add("mm5","mm5","mm5","mm5","mm0","mm2","mm4","mm6")
        "movq   (%2,%%"REG_d"), %%mm1         \n\t"
        "movq  8(%2,%%"REG_d"), %%mm3         \n\t"
        "paddw %%mm7, %%mm1                          \n\t"
        "paddw %%mm7, %%mm3                          \n\t"
        "pavgw %%mm1, %%mm0                          \n\t"
        "pavgw %%mm3, %%mm2                          \n\t"
        "movq 16(%2,%%"REG_d"), %%mm1         \n\t"
        "movq 24(%2,%%"REG_d"), %%mm3         \n\t"
        "paddw %%mm7, %%mm1                          \n\t"
        "paddw %%mm7, %%mm3                          \n\t"
        "pavgw %%mm1, %%mm4                          \n\t"
        "pavgw %%mm3, %%mm6                          \n\t"
        snow_vertical_compose_r2r_sub("mm7","mm7","mm7","mm7","mm0","mm2","mm4","mm6")
        snow_vertical_compose_sra("1","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_add("%3","mm0","mm2","mm4","mm6")

        snow_vertical_compose_sra("2","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_add("%3","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_store("%3","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_add("%1","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_move("mm0","mm2","mm4","mm6","mm1","mm3","mm5","mm7")
        snow_vertical_compose_sra("1","mm0","mm2","mm4","mm6")
        snow_vertical_compose_r2r_add("mm1","mm3","mm5","mm7","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_add("%2","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_store("%2","mm0","mm2","mm4","mm6")

        "2:                                          \n\t"
        "sub $32, %%"REG_d"                          \n\t"
        "jge 1b                                      \n\t"
        :"+d"(i)
        :"r"(b0),"r"(b1),"r"(b2),"r"(b3),"r"(b4),"r"(b5));
}
