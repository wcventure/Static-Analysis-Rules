static av_always_inline SoftFloat autocorr_calc(int64_t accu)
{
        int nz, mant, expo;
        unsigned round;
        int i = (int)(accu >> 32);
        if (i == 0) {
            nz = 1;
        } else {
            nz = 0;
            while (FFABS(i) < 0x40000000) {
                i *= 2;
                nz++;
            }
            nz = 32-nz;
        }

        round = 1U << (nz-1);
        mant = (int)((accu + round) >> nz);
        mant = (mant + 0x40)>>7;
        mant *= 64;
        expo = nz + 15;
        return av_int2sf(mant, 30 - expo);
}
