int ff_mpeg_update_thread_context(AVCodecContext *dst,
                                  const AVCodecContext *src)
{
    int i;
    int err;
    MpegEncContext *s = dst->priv_data, *s1 = src->priv_data;

    if (dst == src)
        return 0;

    av_assert0(s != s1);

    // FIXME can parameters change on I-frames?
    // in that case dst may need a reinit
    if (!s->context_initialized) {
        memcpy(s, s1, sizeof(MpegEncContext));

        s->avctx                 = dst;
        s->bitstream_buffer      = NULL;
        s->bitstream_buffer_size = s->allocated_bitstream_buffer_size = 0;

        if (s1->context_initialized){
            s->picture_range_start  += MAX_PICTURE_COUNT;
            s->picture_range_end    += MAX_PICTURE_COUNT;
            if((err = ff_MPV_common_init(s)) < 0)
                return err;
        }
    }

    if (s->height != s1->height || s->width != s1->width || s->context_reinit) {
        s->context_reinit = 0;
        s->height = s1->height;
        s->width  = s1->width;
        if ((err = ff_MPV_common_frame_size_change(s)) < 0)
            return err;
    }

    s->avctx->coded_height  = s1->avctx->coded_height;
    s->avctx->coded_width   = s1->avctx->coded_width;
    s->avctx->width         = s1->avctx->width;
    s->avctx->height        = s1->avctx->height;

    s->coded_picture_number = s1->coded_picture_number;
    s->picture_number       = s1->picture_number;
    s->input_picture_number = s1->input_picture_number;

    av_assert0(!s->picture || s->picture != s1->picture);
    memcpy(s->picture, s1->picture, s1->picture_count * sizeof(Picture));
    memcpy(&s->last_picture, &s1->last_picture,
           (char *) &s1->last_picture_ptr - (char *) &s1->last_picture);

    // reset s->picture[].f.extended_data to s->picture[].f.data
    for (i = 0; i < s->picture_count; i++)
        s->picture[i].f.extended_data = s->picture[i].f.data;

    s->last_picture_ptr    = REBASE_PICTURE(s1->last_picture_ptr,    s, s1);
    s->current_picture_ptr = REBASE_PICTURE(s1->current_picture_ptr, s, s1);
    s->next_picture_ptr    = REBASE_PICTURE(s1->next_picture_ptr,    s, s1);

    // Error/bug resilience
    s->next_p_frame_damaged = s1->next_p_frame_damaged;
    s->workaround_bugs      = s1->workaround_bugs;
    s->padding_bug_score    = s1->padding_bug_score;

    // MPEG4 timing info
    memcpy(&s->time_increment_bits, &s1->time_increment_bits,
           (char *) &s1->shape - (char *) &s1->time_increment_bits);

    // B-frame info
    s->max_b_frames = s1->max_b_frames;
    s->low_delay    = s1->low_delay;
    s->droppable    = s1->droppable;

    // DivX handling (doesn't work)
    s->divx_packed  = s1->divx_packed;

    if (s1->bitstream_buffer) {
        if (s1->bitstream_buffer_size +
            FF_INPUT_BUFFER_PADDING_SIZE > s->allocated_bitstream_buffer_size)
            av_fast_malloc(&s->bitstream_buffer,
                           &s->allocated_bitstream_buffer_size,
                           s1->allocated_bitstream_buffer_size);
            s->bitstream_buffer_size = s1->bitstream_buffer_size;
        memcpy(s->bitstream_buffer, s1->bitstream_buffer,
               s1->bitstream_buffer_size);
        memset(s->bitstream_buffer + s->bitstream_buffer_size, 0,
               FF_INPUT_BUFFER_PADDING_SIZE);
    }

    // linesize dependend scratch buffer allocation
    if (!s->edge_emu_buffer)
        if (s1->linesize) {
            if (ff_mpv_frame_size_alloc(s, s1->linesize) < 0) {
                av_log(s->avctx, AV_LOG_ERROR, "Failed to allocate context "
                       "scratch buffers.\n");
                return AVERROR(ENOMEM);
            }
        } else {
            av_log(s->avctx, AV_LOG_ERROR, "Context scratch buffers could not "
                   "be allocated due to unknown size.\n");
        }

    // MPEG2/interlacing info
    memcpy(&s->progressive_sequence, &s1->progressive_sequence,
           (char *) &s1->rtp_mode - (char *) &s1->progressive_sequence);

    if (!s1->first_field) {
        s->last_pict_type = s1->pict_type;
        if (s1->current_picture_ptr)
            s->last_lambda_for[s1->pict_type] = s1->current_picture_ptr->f.quality;

        if (s1->pict_type != AV_PICTURE_TYPE_B) {
            s->last_non_b_pict_type = s1->pict_type;
        }
    }

    return 0;
}
