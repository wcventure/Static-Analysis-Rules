static int svq3_decode_frame(AVCodecContext *avctx, void *data,
                             int *got_frame, AVPacket *avpkt)
{
    SVQ3Context *s     = avctx->priv_data;
    H264Context *h     = &s->h;
    int buf_size       = avpkt->size;
    int left;
    uint8_t *buf;
    int ret, m, i;

    /
    if (buf_size == 0) {
        if (s->next_pic->f.data[0] && !h->low_delay && !s->last_frame_output) {
            ret = av_frame_ref(data, &s->next_pic->f);
            if (ret < 0)
                return ret;
            s->last_frame_output = 1;
            *got_frame          = 1;
        }
        return 0;
    }

    h->mb_x = h->mb_y = h->mb_xy = 0;

    if (s->watermark_key) {
        av_fast_padded_malloc(&s->buf, &s->buf_size, buf_size);
        if (!s->buf)
            return AVERROR(ENOMEM);
        memcpy(s->buf, avpkt->data, buf_size);
        buf = s->buf;
    } else {
        buf = avpkt->data;
    }

    init_get_bits(&h->gb, buf, 8 * buf_size);

    if (svq3_decode_slice_header(avctx))
        return -1;

    h->pict_type = h->slice_type;

    if (h->pict_type != AV_PICTURE_TYPE_B)
        FFSWAP(H264Picture*, s->next_pic, s->last_pic);

    av_frame_unref(&s->cur_pic->f);

    /
    s->cur_pic->f.pict_type = h->pict_type;
    s->cur_pic->f.key_frame = (h->pict_type == AV_PICTURE_TYPE_I);

    ret = get_buffer(avctx, s->cur_pic);
    if (ret < 0)
        return ret;

    h->cur_pic_ptr = s->cur_pic;
    av_frame_unref(&h->cur_pic.f);
    h->cur_pic     = *s->cur_pic;
    ret = av_frame_ref(&h->cur_pic.f, &s->cur_pic->f);
    if (ret < 0)
        return ret;

    for (i = 0; i < 16; i++) {
        h->block_offset[i]           = (4 * ((scan8[i] - scan8[0]) & 7)) + 4 * h->linesize * ((scan8[i] - scan8[0]) >> 3);
        h->block_offset[48 + i]      = (4 * ((scan8[i] - scan8[0]) & 7)) + 8 * h->linesize * ((scan8[i] - scan8[0]) >> 3);
    }
    for (i = 0; i < 16; i++) {
        h->block_offset[16 + i]      =
        h->block_offset[32 + i]      = (4 * ((scan8[i] - scan8[0]) & 7)) + 4 * h->uvlinesize * ((scan8[i] - scan8[0]) >> 3);
        h->block_offset[48 + 16 + i] =
        h->block_offset[48 + 32 + i] = (4 * ((scan8[i] - scan8[0]) & 7)) + 8 * h->uvlinesize * ((scan8[i] - scan8[0]) >> 3);
    }

    if (h->pict_type != AV_PICTURE_TYPE_I) {
        if (!s->last_pic->f.data[0]) {
            av_log(avctx, AV_LOG_ERROR, "Missing reference frame.\n");
            av_frame_unref(&s->last_pic->f);
            ret = get_buffer(avctx, s->last_pic);
            if (ret < 0)
                return ret;
            memset(s->last_pic->f.data[0], 0, avctx->height * s->last_pic->f.linesize[0]);
            memset(s->last_pic->f.data[1], 0x80, (avctx->height / 2) *
                   s->last_pic->f.linesize[1]);
            memset(s->last_pic->f.data[2], 0x80, (avctx->height / 2) *
                   s->last_pic->f.linesize[2]);
        }

        if (h->pict_type == AV_PICTURE_TYPE_B && !s->next_pic->f.data[0]) {
            av_log(avctx, AV_LOG_ERROR, "Missing reference frame.\n");
            av_frame_unref(&s->next_pic->f);
            ret = get_buffer(avctx, s->next_pic);
            if (ret < 0)
                return ret;
            memset(s->next_pic->f.data[0], 0, avctx->height * s->next_pic->f.linesize[0]);
            memset(s->next_pic->f.data[1], 0x80, (avctx->height / 2) *
                   s->next_pic->f.linesize[1]);
            memset(s->next_pic->f.data[2], 0x80, (avctx->height / 2) *
                   s->next_pic->f.linesize[2]);
        }
    }

    if (avctx->debug & FF_DEBUG_PICT_INFO)
        av_log(h->avctx, AV_LOG_DEBUG,
               "%c hpel:%d, tpel:%d aqp:%d qp:%d, slice_num:%02X\n",
               av_get_picture_type_char(h->pict_type),
               s->halfpel_flag, s->thirdpel_flag,
               s->adaptive_quant, h->qscale, h->slice_num);

    if (avctx->skip_frame >= AVDISCARD_NONREF && h->pict_type == AV_PICTURE_TYPE_B ||
        avctx->skip_frame >= AVDISCARD_NONKEY && h->pict_type != AV_PICTURE_TYPE_I ||
        avctx->skip_frame >= AVDISCARD_ALL)
        return 0;

    if (s->next_p_frame_damaged) {
        if (h->pict_type == AV_PICTURE_TYPE_B)
            return 0;
        else
            s->next_p_frame_damaged = 0;
    }

    if (h->pict_type == AV_PICTURE_TYPE_B) {
        h->frame_num_offset = h->slice_num - h->prev_frame_num;

        if (h->frame_num_offset < 0)
            h->frame_num_offset += 256;
        if (h->frame_num_offset == 0 ||
            h->frame_num_offset >= h->prev_frame_num_offset) {
            av_log(h->avctx, AV_LOG_ERROR, "error in B-frame picture id\n");
            return -1;
        }
    } else {
        h->prev_frame_num        = h->frame_num;
        h->frame_num             = h->slice_num;
        h->prev_frame_num_offset = h->frame_num - h->prev_frame_num;

        if (h->prev_frame_num_offset < 0)
            h->prev_frame_num_offset += 256;
    }

    for (m = 0; m < 2; m++) {
        int i;
        for (i = 0; i < 4; i++) {
            int j;
            for (j = -1; j < 4; j++)
                h->ref_cache[m][scan8[0] + 8 * i + j] = 1;
            if (i < 3)
                h->ref_cache[m][scan8[0] + 8 * i + j] = PART_NOT_AVAILABLE;
        }
    }

    for (h->mb_y = 0; h->mb_y < h->mb_height; h->mb_y++) {
        for (h->mb_x = 0; h->mb_x < h->mb_width; h->mb_x++) {
            unsigned mb_type;
            h->mb_xy = h->mb_x + h->mb_y * h->mb_stride;

            if ((get_bits_count(&h->gb) + 7) >= h->gb.size_in_bits &&
                ((get_bits_count(&h->gb) & 7) == 0 ||
                 show_bits(&h->gb, -get_bits_count(&h->gb) & 7) == 0)) {
                skip_bits(&h->gb, s->next_slice_index - get_bits_count(&h->gb));
                h->gb.size_in_bits = 8 * buf_size;

                if (svq3_decode_slice_header(avctx))
                    return -1;

                /
            }

            mb_type = svq3_get_ue_golomb(&h->gb);

            if (h->pict_type == AV_PICTURE_TYPE_I)
                mb_type += 8;
            else if (h->pict_type == AV_PICTURE_TYPE_B && mb_type >= 4)
                mb_type += 4;
            if (mb_type > 33 || svq3_decode_mb(s, mb_type)) {
                av_log(h->avctx, AV_LOG_ERROR,
                       "error while decoding MB %d %d\n", h->mb_x, h->mb_y);
                return -1;
            }

            if (mb_type != 0 || h->cbp)
                ff_h264_hl_decode_mb(h);

            if (h->pict_type != AV_PICTURE_TYPE_B && !h->low_delay)
                h->cur_pic.mb_type[h->mb_x + h->mb_y * h->mb_stride] =
                    (h->pict_type == AV_PICTURE_TYPE_P && mb_type < 8) ? (mb_type - 1) : -1;
        }

        ff_draw_horiz_band(avctx, &s->cur_pic->f,
                           s->last_pic->f.data[0] ? &s->last_pic->f : NULL,
                           16 * h->mb_y, 16, h->picture_structure, 0,
                           h->low_delay);
    }

    left = buf_size*8 - get_bits_count(&h->gb);

    if (h->mb_y != h->mb_height || h->mb_x != h->mb_width) {
        av_log(avctx, AV_LOG_INFO, "frame num %d incomplete pic x %d y %d left %d\n", avctx->frame_number, h->mb_y, h->mb_x, left);
        //av_hex_dump(stderr, buf+buf_size-8, 8);
    }

    if (left < 0) {
        av_log(avctx, AV_LOG_ERROR, "frame num %d left %d\n", avctx->frame_number, left);
        return -1;
    }

    if (h->pict_type == AV_PICTURE_TYPE_B || h->low_delay)
        ret = av_frame_ref(data, &s->cur_pic->f);
    else if (s->last_pic->f.data[0])
        ret = av_frame_ref(data, &s->last_pic->f);
    if (ret < 0)
        return ret;

    /
    if (s->last_pic->f.data[0] || h->low_delay)
        *got_frame = 1;

    if (h->pict_type != AV_PICTURE_TYPE_B) {
        FFSWAP(H264Picture*, s->cur_pic, s->next_pic);
    } else {
        av_frame_unref(&s->cur_pic->f);
    }

    return buf_size;
}
