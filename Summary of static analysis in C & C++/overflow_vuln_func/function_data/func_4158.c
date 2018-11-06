static void mpeg_decode_sequence_extension(Mpeg1Context *s1)
{
    MpegEncContext *s = &s1->mpeg_enc_ctx;
    int horiz_size_ext, vert_size_ext;
    int bit_rate_ext;

    skip_bits(&s->gb, 1); /
    s->avctx->profile       = get_bits(&s->gb, 3);
    s->avctx->level         = get_bits(&s->gb, 4);
    s->progressive_sequence = get_bits1(&s->gb);   /
    s->chroma_format        = get_bits(&s->gb, 2); /

    if (!s->chroma_format) {
        s->chroma_format = 1;
        av_log(s->avctx, AV_LOG_WARNING, "Chroma format invalid\n");
    }

    horiz_size_ext          = get_bits(&s->gb, 2);
    vert_size_ext           = get_bits(&s->gb, 2);
    s->width  |= (horiz_size_ext << 12);
    s->height |= (vert_size_ext  << 12);
    bit_rate_ext = get_bits(&s->gb, 12);  /
    s->bit_rate += (bit_rate_ext << 18) * 400;
    check_marker(&s->gb, "after bit rate extension");
    s->avctx->rc_buffer_size += get_bits(&s->gb, 8) * 1024 * 16 << 10;

    s->low_delay = get_bits1(&s->gb);
    if (s->avctx->flags & AV_CODEC_FLAG_LOW_DELAY)
        s->low_delay = 1;

    s1->frame_rate_ext.num = get_bits(&s->gb, 2) + 1;
    s1->frame_rate_ext.den = get_bits(&s->gb, 5) + 1;

    ff_dlog(s->avctx, "sequence extension\n");
    s->codec_id = s->avctx->codec_id = AV_CODEC_ID_MPEG2VIDEO;

    if (s->avctx->debug & FF_DEBUG_PICT_INFO)
        av_log(s->avctx, AV_LOG_DEBUG,
               "profile: %d, level: %d ps: %d cf:%d vbv buffer: %d, bitrate:%"PRId64"\n",
               s->avctx->profile, s->avctx->level, s->progressive_sequence, s->chroma_format,
               s->avctx->rc_buffer_size, s->bit_rate);
}
