av_cold int ff_h264_decode_init(AVCodecContext *avctx)
{
    H264Context *h = avctx->priv_data;
    MpegEncContext *const s = &h->s;
    int i;

    ff_MPV_decode_defaults(s);

    s->avctx = avctx;
    common_init(h);

    s->out_format      = FMT_H264;
    s->workaround_bugs = avctx->workaround_bugs;

    /
    // s->decode_mb = ff_h263_decode_mb;
    s->quarter_sample = 1;
    if (!avctx->has_b_frames)
        s->low_delay = 1;

    avctx->chroma_sample_location = AVCHROMA_LOC_LEFT;

    ff_h264_decode_init_vlc();

    h->pixel_shift = 0;
    h->sps.bit_depth_luma = avctx->bits_per_raw_sample = 8;

    h->thread_context[0] = h;
    h->outputed_poc      = h->next_outputed_poc = INT_MIN;
    for (i = 0; i < MAX_DELAYED_PIC_COUNT; i++)
        h->last_pocs[i] = INT_MIN;
    h->prev_poc_msb = 1 << 16;
    h->prev_frame_num = -1;
    h->x264_build   = -1;
    ff_h264_reset_sei(h);
    if (avctx->codec_id == AV_CODEC_ID_H264) {
        if (avctx->ticks_per_frame == 1)
            s->avctx->time_base.den *= 2;
        avctx->ticks_per_frame = 2;
    }

    if (avctx->extradata_size > 0 && avctx->extradata &&
        ff_h264_decode_extradata(h, avctx->extradata, avctx->extradata_size) < 0) {
        ff_h264_free_context(h);
        return -1;
    }

    if (h->sps.bitstream_restriction_flag &&
        s->avctx->has_b_frames < h->sps.num_reorder_frames) {
        s->avctx->has_b_frames = h->sps.num_reorder_frames;
        s->low_delay           = 0;
    }

    ff_init_cabac_states();

    return 0;
}
