static int h264_parse(AVCodecParserContext *s,
                      AVCodecContext *avctx,
                      const uint8_t **poutbuf, int *poutbuf_size,
                      const uint8_t *buf, int buf_size)
{
    H264ParseContext *p = s->priv_data;
    ParseContext *pc = &p->pc;
    int next;

    if (!p->got_first) {
        p->got_first = 1;
        if (avctx->extradata_size) {
            ff_h264_decode_extradata(avctx->extradata, avctx->extradata_size,
                                     &p->ps, &p->is_avc, &p->nal_length_size,
                                     avctx->err_recognition, avctx);
        }
    }

    if (s->flags & PARSER_FLAG_COMPLETE_FRAMES) {
        next = buf_size;
    } else {
        next = h264_find_frame_end(p, buf, buf_size, avctx);

        if (ff_combine_frame(pc, next, &buf, &buf_size) < 0) {
            *poutbuf      = NULL;
            *poutbuf_size = 0;
            return buf_size;
        }

        if (next < 0 && next != END_NOT_FOUND) {
            av_assert1(pc->last_index + next >= 0);
            h264_find_frame_end(p, &pc->buffer[pc->last_index + next], -next, avctx); // update state
        }
    }

    parse_nal_units(s, avctx, buf, buf_size);

    if (avctx->framerate.num)
        avctx->time_base = av_inv_q(av_mul_q(avctx->framerate, (AVRational){avctx->ticks_per_frame, 1}));
    if (p->sei.picture_timing.cpb_removal_delay >= 0) {
        s->dts_sync_point    = p->sei.buffering_period.present;
        s->dts_ref_dts_delta = p->sei.picture_timing.cpb_removal_delay;
        s->pts_dts_delta     = p->sei.picture_timing.dpb_output_delay;
    } else {
        s->dts_sync_point    = INT_MIN;
        s->dts_ref_dts_delta = INT_MIN;
        s->pts_dts_delta     = INT_MIN;
    }

    if (s->flags & PARSER_FLAG_ONCE) {
        s->flags &= PARSER_FLAG_COMPLETE_FRAMES;
    }

    if (s->dts_sync_point >= 0) {
        int64_t den = avctx->time_base.den * avctx->pkt_timebase.num;
        if (den > 0) {
            int64_t num = avctx->time_base.num * avctx->pkt_timebase.den;
            if (s->dts != AV_NOPTS_VALUE) {
                // got DTS from the stream, update reference timestamp
                p->reference_dts = s->dts - av_rescale(s->dts_ref_dts_delta, num, den);
            } else if (p->reference_dts != AV_NOPTS_VALUE) {
                // compute DTS based on reference timestamp
                s->dts = p->reference_dts + av_rescale(s->dts_ref_dts_delta, num, den);
            }

            if (p->reference_dts != AV_NOPTS_VALUE && s->pts == AV_NOPTS_VALUE)
                s->pts = s->dts + av_rescale(s->pts_dts_delta, num, den);

            if (s->dts_sync_point > 0)
                p->reference_dts = s->dts; // new reference
        }
    }

    *poutbuf      = buf;
    *poutbuf_size = buf_size;
    return next;
}
