static void adx_decode(ADXContext *c, int16_t *out, const uint8_t *in, int ch)
{
    ADXChannelState *prev = &c->prev[ch];
    GetBitContext gb;
    int scale = AV_RB16(in);
    int i;
    int s0, s1, s2, d;

    init_get_bits(&gb, in + 2, (18 - 2) * 8);
    s1 = prev->s1;
    s2 = prev->s2;
    for (i = 0; i < 32; i++) {
        d  = get_sbits(&gb, 4);
        s0 = (BASEVOL * d * scale + SCALE1 * s1 - SCALE2 * s2) >> 14;
        s2 = s1;
        s1 = av_clip_int16(s0);
        *out = s1;
        out += c->channels;
    }
    prev->s1 = s1;
    prev->s2 = s2;
}
