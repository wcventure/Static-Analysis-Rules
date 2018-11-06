static void apply_window_and_mdct(AACEncContext *s, SingleChannelElement *sce,
                                  float *audio)
{
    int i;
    float *output = sce->ret_buf;

    apply_window[sce->ics.window_sequence[0]](s->fdsp, sce, audio);

    if (sce->ics.window_sequence[0] != EIGHT_SHORT_SEQUENCE)
        s->mdct1024.mdct_calc(&s->mdct1024, sce->coeffs, output);
    else
        for (i = 0; i < 1024; i += 128)
            s->mdct128.mdct_calc(&s->mdct128, sce->coeffs + i, output + i*2);
    memcpy(audio, audio + 1024, sizeof(audio[0]) * 1024);
    memcpy(sce->pcoeffs, sce->coeffs, sizeof(sce->pcoeffs));
}
