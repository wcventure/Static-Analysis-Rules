static void set_sar(TiffContext *s, unsigned tag, unsigned num, unsigned den)
{
    int offset = tag == TIFF_YRES ? 2 : 0;
    s->res[offset++] = num;
    s->res[offset]   = den;
    if (s->res[0] && s->res[1] && s->res[2] && s->res[3])
        av_reduce(&s->avctx->sample_aspect_ratio.num, &s->avctx->sample_aspect_ratio.den,
                  s->res[2] * (uint64_t)s->res[1], s->res[0] * (uint64_t)s->res[3], INT32_MAX);
}
