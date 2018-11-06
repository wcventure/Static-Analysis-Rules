void ff_snow_vertical_compose97i_mmx(DWTELEM *b0, DWTELEM *b1, DWTELEM *b2, DWTELEM *b3, DWTELEM *b4, DWTELEM *b5, int width){
    long i = width;
    while(i & 0x7)
    {
        i--;
        b4[i] -= (W_DM*(b3[i] + b5[i])+W_DO)>>W_DS;
        b3[i] -= (W_CM*(b2[i] + b4[i])+W_CO)>>W_CS;
        b2[i] += (W_BM*(b1[i] + b3[i])+4*b2[i]+W_BO)>>W_BS;
        b1[i] += (W_AM*(b0[i] + b2[i])+W_AO)>>W_AS;
    }

    asm volatile(
        "jmp 2f                                      \n\t"
        "1:                                          \n\t"

        "mov %6, %%"REG_a"                           \n\t"
        "mov %4, %%"REG_S"                           \n\t"

        snow_vertical_compose_mmx_load(REG_S,"mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_add(REG_a,"mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_move("mm0","mm2","mm4","mm6","mm1","mm3","mm5","mm7")
        snow_vertical_compose_mmx_r2r_add("mm0","mm2","mm4","mm6","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_r2r_add("mm1","mm3","mm5","mm7","mm0","mm2","mm4","mm6")

        "pcmpeqd %%mm1, %%mm1                        \n\t"
        "pslld $31, %%mm1                            \n\t"
        "psrld $29, %%mm1                            \n\t"
        "mov %5, %%"REG_a"                           \n\t"

        snow_vertical_compose_mmx_r2r_add("mm1","mm1","mm1","mm1","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_sra("3","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_load(REG_a,"mm1","mm3","mm5","mm7")
        snow_vertical_compose_mmx_sub("mm0","mm2","mm4","mm6","mm1","mm3","mm5","mm7")
        snow_vertical_compose_mmx_store(REG_a,"mm1","mm3","mm5","mm7")
        "mov %3, %%"REG_c"                           \n\t"
        snow_vertical_compose_mmx_load(REG_S,"mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_add(REG_c,"mm1","mm3","mm5","mm7")
        snow_vertical_compose_mmx_sub("mm1","mm3","mm5","mm7","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_store(REG_S,"mm0","mm2","mm4","mm6")
        "mov %2, %%"REG_a"                           \n\t"
        snow_vertical_compose_mmx_add(REG_a,"mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_sra("2","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_add(REG_c,"mm0","mm2","mm4","mm6")

        "pcmpeqd %%mm1, %%mm1                        \n\t"
        "pslld $31, %%mm1                            \n\t"
        "psrld $30, %%mm1                            \n\t"
        "mov %1, %%"REG_S"                           \n\t"

        snow_vertical_compose_mmx_r2r_add("mm1","mm1","mm1","mm1","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_sra("2","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_add(REG_c,"mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_store(REG_c,"mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_add(REG_S,"mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_move("mm0","mm2","mm4","mm6","mm1","mm3","mm5","mm7")
        snow_vertical_compose_mmx_sra("1","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_r2r_add("mm1","mm3","mm5","mm7","mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_add(REG_a,"mm0","mm2","mm4","mm6")
        snow_vertical_compose_mmx_store(REG_a,"mm0","mm2","mm4","mm6")

        "2:                                          \n\t"
        "sub $8, %%"REG_d"                           \n\t"
        "jge 1b                                      \n\t"
        :"+d"(i)
        :
        "m"(b0),"m"(b1),"m"(b2),"m"(b3),"m"(b4),"m"(b5):
        "%"REG_a"","%"REG_S"","%"REG_c"");
}
