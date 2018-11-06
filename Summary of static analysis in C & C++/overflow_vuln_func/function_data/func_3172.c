static int dirac_decode_frame(AVCodecContext *avctx, void *data, int *got_frame, AVPacket *pkt)
{
    DiracContext *s     = avctx->priv_data;
    AVFrame *picture    = data;
    uint8_t *buf        = pkt->data;
    int buf_size        = pkt->size;
    int i, data_unit_size, buf_idx = 0;
    int ret;

    /
    for (i = 0; i < MAX_FRAMES; i++)
        if (s->all_frames[i].avframe->data[0] && !s->all_frames[i].avframe->reference) {
            av_frame_unref(s->all_frames[i].avframe);
            memset(s->all_frames[i].interpolated, 0, sizeof(s->all_frames[i].interpolated));
        }

    s->current_picture = NULL;
    *got_frame = 0;

    /
    if (buf_size == 0)
        return get_delayed_pic(s, (AVFrame *)data, got_frame);

    for (;;) {
        /
        for (; buf_idx + DATA_UNIT_HEADER_SIZE < buf_size; buf_idx++) {
            if (buf[buf_idx  ] == 'B' && buf[buf_idx+1] == 'B' &&
                buf[buf_idx+2] == 'C' && buf[buf_idx+3] == 'D')
                break;
        }
        /
        if (buf_idx + DATA_UNIT_HEADER_SIZE >= buf_size)
            break;

        data_unit_size = AV_RB32(buf+buf_idx+5);
        if (buf_idx + data_unit_size > buf_size || !data_unit_size) {
            if(buf_idx + data_unit_size > buf_size)
            av_log(s->avctx, AV_LOG_ERROR,
                   "Data unit with size %d is larger than input buffer, discarding\n",
                   data_unit_size);
            buf_idx += 4;
            continue;
        }
        /
        if (dirac_decode_data_unit(avctx, buf+buf_idx, data_unit_size))
        {
            av_log(s->avctx, AV_LOG_ERROR,"Error in dirac_decode_data_unit\n");
            return -1;
        }
        buf_idx += data_unit_size;
    }

    if (!s->current_picture)
        return buf_size;

    if (s->current_picture->avframe->display_picture_number > s->frame_number) {
        DiracFrame *delayed_frame = remove_frame(s->delay_frames, s->frame_number);

        s->current_picture->avframe->reference |= DELAYED_PIC_REF;

        if (add_frame(s->delay_frames, MAX_DELAY, s->current_picture)) {
            int min_num = s->delay_frames[0]->avframe->display_picture_number;
            /
            av_log(avctx, AV_LOG_ERROR, "Delay frame overflow\n");

            for (i = 1; s->delay_frames[i]; i++)
                if (s->delay_frames[i]->avframe->display_picture_number < min_num)
                    min_num = s->delay_frames[i]->avframe->display_picture_number;

            delayed_frame = remove_frame(s->delay_frames, min_num);
            add_frame(s->delay_frames, MAX_DELAY, s->current_picture);
        }

        if (delayed_frame) {
            delayed_frame->avframe->reference ^= DELAYED_PIC_REF;
            if((ret=av_frame_ref(data, delayed_frame->avframe)) < 0)
                return ret;
            *got_frame = 1;
        }
    } else if (s->current_picture->avframe->display_picture_number == s->frame_number) {
        /
        if((ret=av_frame_ref(data, s->current_picture->avframe)) < 0)
            return ret;
        *got_frame = 1;
    }

    if (*got_frame)
        s->frame_number = picture->display_picture_number + 1;

    return buf_idx;
}
