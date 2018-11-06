static void apply_mdct(AC3EncodeContext *s)
{
    int blk, ch;

    for (ch = 0; ch < s->channels; ch++) {
        for (blk = 0; blk < AC3_MAX_BLOCKS; blk++) {
            AC3Block *block = &s->blocks[blk];
            const SampleType *input_samples = &s->planar_samples[ch][blk * AC3_BLOCK_SIZE];

            apply_window(&s->dsp, s->windowed_samples, input_samples, s->mdct.window, AC3_WINDOW_SIZE);

            block->exp_shift[ch] = normalize_samples(s);

            mdct512(&s->mdct, block->mdct_coef[ch], s->windowed_samples);
        }
    }
}
