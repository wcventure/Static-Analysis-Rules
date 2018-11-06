static SoftFloat sbr_sum_square_c(int (*x)[2], int n)
{
    SoftFloat ret;
    int64_t accu = 0;
    int i, nz, round;

    for (i = 0; i < n; i += 2) {
        // Larger values are inavlid and could cause overflows of accu.
        av_assert2(FFABS(x[i + 0][0]) >> 29 == 0);
        accu += (int64_t)x[i + 0][0] * x[i + 0][0];
        av_assert2(FFABS(x[i + 0][1]) >> 29 == 0);
        accu += (int64_t)x[i + 0][1] * x[i + 0][1];
        av_assert2(FFABS(x[i + 1][0]) >> 29 == 0);
        accu += (int64_t)x[i + 1][0] * x[i + 1][0];
        av_assert2(FFABS(x[i + 1][1]) >> 29 == 0);
        accu += (int64_t)x[i + 1][1] * x[i + 1][1];
    }

    i = (int)(accu >> 32);
    if (i == 0) {
        nz = 1;
    } else {
        nz = 0;
        while (FFABS(i) < 0x40000000) {
            i <<= 1;
            nz++;
        }
        nz = 32 - nz;
    }

    round = 1 << (nz-1);
    i = (int)((accu + round) >> nz);
    i >>= 1;
    ret = av_int2sf(i, 15 - nz);

    return ret;
}
