static int normalize_samples(AC3EncodeContext *s)
{
    int v = 14 - log2_tab(s, s->windowed_samples, AC3_WINDOW_SIZE);
    lshift_tab(s->windowed_samples, AC3_WINDOW_SIZE, v);
    return v - 9;
}
