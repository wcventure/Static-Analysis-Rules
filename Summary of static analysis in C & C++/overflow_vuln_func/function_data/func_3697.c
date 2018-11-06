static void extract_exponents(AC3EncodeContext *s)
{
    int blk, ch, i;

    for (ch = 0; ch < s->channels; ch++) {
        for (blk = 0; blk < AC3_MAX_BLOCKS; blk++) {
            AC3Block *block = &s->blocks[blk];
            uint8_t *exp   = block->exp[ch];
            int32_t *coef = block->fixed_coef[ch];
            int exp_shift  = block->exp_shift[ch];
            for (i = 0; i < AC3_MAX_COEFS; i++) {
                int e;
                int v = abs(coef[i]);
                if (v == 0)
                    e = 24;
                else {
                    e = 23 - av_log2(v) + exp_shift;
                    if (e >= 24) {
                        e = 24;
                        coef[i] = 0;
                    }
                    av_assert2(e >= 0);
                }
                exp[i] = e;
            }
        }
    }
}
