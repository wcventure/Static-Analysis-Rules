static void cmv_process_header(CmvContext *s, const uint8_t *buf, const uint8_t *buf_end)
{
    int pal_start, pal_count, i;

    if(buf_end - buf < 16) {
        av_log(s->avctx, AV_LOG_WARNING, "truncated header\n");
        return;
    }

    s->width  = AV_RL16(&buf[4]);
    s->height = AV_RL16(&buf[6]);
    if (s->avctx->width!=s->width || s->avctx->height!=s->height)
        avcodec_set_dimensions(s->avctx, s->width, s->height);

    s->avctx->time_base.num = 1;
    s->avctx->time_base.den = AV_RL16(&buf[10]);

    pal_start = AV_RL16(&buf[12]);
    pal_count = AV_RL16(&buf[14]);

    buf += 16;
    for (i=pal_start; i<pal_start+pal_count && i<AVPALETTE_COUNT && buf_end - buf >= 3; i++) {
        s->palette[i] = 0xFF << 24 | AV_RB24(buf);
        buf += 3;
    }
}
