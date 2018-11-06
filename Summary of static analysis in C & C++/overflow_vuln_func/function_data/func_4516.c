static void do_apply_filter(APEContext *ctx, int version, APEFilter *f,
                            int32_t *data, int count, int order, int fracbits)
{
    int res;
    int absres;

    while (count--) {
        /
        res = ctx->dsp.scalarproduct_and_madd_int16(f->coeffs, f->delay - order,
                                                    f->adaptcoeffs - order,
                                                    order, APESIGN(*data));
        res = (res + (1 << (fracbits - 1))) >> fracbits;
        res += *data;
        *data++ = res;

        /
        *f->delay++ = av_clip_int16(res);

        if (version < 3980) {
            /
            f->adaptcoeffs[0]  = (res == 0) ? 0 : ((res >> 28) & 8) - 4;
            f->adaptcoeffs[-4] >>= 1;
            f->adaptcoeffs[-8] >>= 1;
        } else {
            /

            /
            absres = FFABS(res);
            if (absres)
                *f->adaptcoeffs = ((res & (1<<31)) - (1<<30)) >>
                                  (25 + (absres <= f->avg*3) + (absres <= f->avg*4/3));
            else
                *f->adaptcoeffs = 0;

            f->avg += (absres - f->avg) / 16;

            f->adaptcoeffs[-1] >>= 1;
            f->adaptcoeffs[-2] >>= 1;
            f->adaptcoeffs[-8] >>= 1;
        }

        f->adaptcoeffs++;

        /
        if (f->delay == f->historybuffer + HISTORY_SIZE + (order * 2)) {
            memmove(f->historybuffer, f->delay - (order * 2),
                    (order * 2) * sizeof(int16_t));
            f->delay = f->historybuffer + order * 2;
            f->adaptcoeffs = f->historybuffer + order;
        }
    }
}
