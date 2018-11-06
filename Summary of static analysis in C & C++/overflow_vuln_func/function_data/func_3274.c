static av_always_inline void fic_idct(int16_t *blk, int step, int shift, int rnd)
{
    const int t0 =  27246 * blk[3 * step] + 18405 * blk[5 * step];
    const int t1 =  27246 * blk[5 * step] - 18405 * blk[3 * step];
    const int t2 =   6393 * blk[7 * step] + 32139 * blk[1 * step];
    const int t3 =   6393 * blk[1 * step] - 32139 * blk[7 * step];
    const int t4 = 5793 * (t2 + t0 + 0x800 >> 12);
    const int t5 = 5793 * (t3 + t1 + 0x800 >> 12);
    const int t6 = t2 - t0;
    const int t7 = t3 - t1;
    const int t8 =  17734 * blk[2 * step] - 42813 * blk[6 * step];
    const int t9 =  17734 * blk[6 * step] + 42814 * blk[2 * step];
    const int tA = (blk[0 * step] - blk[4 * step]) * 32768 + rnd;
    const int tB = (blk[0 * step] + blk[4 * step]) * 32768 + rnd;
    blk[0 * step] = (  t4       + t9 + tB) >> shift;
    blk[1 * step] = (  t6 + t7  + t8 + tA) >> shift;
    blk[2 * step] = (  t6 - t7  - t8 + tA) >> shift;
    blk[3 * step] = (  t5       - t9 + tB) >> shift;
    blk[4 * step] = ( -t5       - t9 + tB) >> shift;
    blk[5 * step] = (-(t6 - t7) - t8 + tA) >> shift;
    blk[6 * step] = (-(t6 + t7) + t8 + tA) >> shift;
    blk[7 * step] = ( -t4       + t9 + tB) >> shift;
}
