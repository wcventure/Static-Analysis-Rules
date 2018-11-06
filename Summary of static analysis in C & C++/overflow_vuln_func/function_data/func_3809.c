static av_always_inline void fic_idct(int16_t *blk, int step, int shift, int rnd)
{
    const int t0 =  27246 * blk[3 * step] + 18405 * blk[5 * step];
    const int t1 =  27246 * blk[5 * step] - 18405 * blk[3 * step];
    const int t2 =   6393 * blk[7 * step] + 32139 * blk[1 * step];
    const int t3 =   6393 * blk[1 * step] - 32139 * blk[7 * step];
    const unsigned t4 = 5793U * (t2 + t0 + 0x800 >> 12);
    const unsigned t5 = 5793U * (t3 + t1 + 0x800 >> 12);
    const unsigned t6 = t2 - t0;
    const unsigned t7 = t3 - t1;
    const unsigned t8 =  17734 * blk[2 * step] - 42813 * blk[6 * step];
    const unsigned t9 =  17734 * blk[6 * step] + 42814 * blk[2 * step];
    const unsigned tA = (blk[0 * step] - blk[4 * step]) * 32768 + rnd;
    const unsigned tB = (blk[0 * step] + blk[4 * step]) * 32768 + rnd;
    blk[0 * step] = (int)(  t4       + t9 + tB) >> shift;
    blk[1 * step] = (int)(  t6 + t7  + t8 + tA) >> shift;
    blk[2 * step] = (int)(  t6 - t7  - t8 + tA) >> shift;
    blk[3 * step] = (int)(  t5       - t9 + tB) >> shift;
    blk[4 * step] = (int)( -t5       - t9 + tB) >> shift;
    blk[5 * step] = (int)(-(t6 - t7) - t8 + tA) >> shift;
    blk[6 * step] = (int)(-(t6 + t7) + t8 + tA) >> shift;
    blk[7 * step] = (int)( -t4       + t9 + tB) >> shift;
}
