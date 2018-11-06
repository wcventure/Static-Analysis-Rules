static void sbr_qmf_deint_bfly_c(INTFLOAT *v, const INTFLOAT *src0, const INTFLOAT *src1)
{
    int i;
    for (i = 0; i < 64; i++) {
        v[      i] = AAC_SRA_R((src0[i] - src1[63 - i]), 5);
        v[127 - i] = AAC_SRA_R((src0[i] + src1[63 - i]), 5);
    }
}
