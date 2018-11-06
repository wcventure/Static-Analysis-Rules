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

         asm volatile (
        "jmp 2f                                      \n\t"
        "1:                                          \n\t"

        "mov %6, %%"REG_a"                           \n\t"
        "mov %4, %%"REG_S"                           \n\t"

        snow_vertical_compose_sse2_load(REG_S,"xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_add(REG_a,"xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_move("xmm0","xmm2","xmm4","xmm6","xmm1","xmm3","xmm5","xmm7")
        snow_vertical_compose_sse2_sra("1","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_r2r_add("xmm1","xmm3","xmm5","xmm7","xmm0","xmm2","xmm4","xmm6")

        "pcmpeqd %%xmm1, %%xmm1                      \n\t"
        "psllw $15, %%xmm1                           \n\t"
        "psrlw $14, %%xmm1                           \n\t"
        "mov %5, %%"REG_a"                           \n\t"

        snow_vertical_compose_sse2_r2r_add("xmm1","xmm1","xmm1","xmm1","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_sra("2","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_load(REG_a,"xmm1","xmm3","xmm5","xmm7")
        snow_vertical_compose_sse2_sub("xmm0","xmm2","xmm4","xmm6","xmm1","xmm3","xmm5","xmm7")
        snow_vertical_compose_sse2_store(REG_a,"xmm1","xmm3","xmm5","xmm7")
        "mov %3, %%"REG_c"                           \n\t"
        snow_vertical_compose_sse2_load(REG_S,"xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_add(REG_c,"xmm1","xmm3","xmm5","xmm7")
        snow_vertical_compose_sse2_sub("xmm1","xmm3","xmm5","xmm7","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_store(REG_S,"xmm0","xmm2","xmm4","xmm6")
        "mov %2, %%"REG_a"                           \n\t"
        snow_vertical_compose_sse2_add(REG_a,"xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_sra("2","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_add(REG_c,"xmm0","xmm2","xmm4","xmm6")

        "pcmpeqd %%xmm1, %%xmm1                      \n\t"
        "psllw $15, %%xmm1                           \n\t"
        "psrlw $14, %%xmm1                           \n\t"
        "mov %1, %%"REG_S"                           \n\t"

        snow_vertical_compose_sse2_r2r_add("xmm1","xmm1","xmm1","xmm1","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_sra("2","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_add(REG_c,"xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_store(REG_c,"xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_add(REG_S,"xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_move("xmm0","xmm2","xmm4","xmm6","xmm1","xmm3","xmm5","xmm7")
        snow_vertical_compose_sse2_sra("1","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_r2r_add("xmm1","xmm3","xmm5","xmm7","xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_add(REG_a,"xmm0","xmm2","xmm4","xmm6")
        snow_vertical_compose_sse2_store(REG_a,"xmm0","xmm2","xmm4","xmm6")

        "2:                                          \n\t"
        "sub $32, %%"REG_d"                          \n\t"
        "jge 1b                                      \n\t"
        :"+d"(i)
        :
        "m"(b0),"m"(b1),"m"(b2),"m"(b3),"m"(b4),"m"(b5):
        "%"REG_a"","%"REG_S"","%"REG_c"");
}
