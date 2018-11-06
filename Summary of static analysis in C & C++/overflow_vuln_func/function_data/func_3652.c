static void mpegvideo_extract_headers(AVCodecParserContext *s,
                                      AVCodecContext *avctx,
                                      const uint8_t *buf, int buf_size)
{
    struct MpvParseContext *pc = s->priv_data;
    const uint8_t *buf_end = buf + buf_size;
    uint32_t start_code;
    int frame_rate_index, ext_type, bytes_left;
    int frame_rate_ext_n, frame_rate_ext_d;
    int top_field_first, repeat_first_field, progressive_frame;
    int horiz_size_ext, vert_size_ext, bit_rate_ext;
    int did_set_size=0;
    int set_dim_ret = 0;
    int bit_rate = 0;
    int vbv_delay = 0;
    int chroma_format;
    enum AVPixelFormat pix_fmt = AV_PIX_FMT_NONE;
//FIXME replace the crap with get_bits()
    s->repeat_pict = 0;

    while (buf < buf_end) {
        start_code= -1;
        buf= avpriv_find_start_code(buf, buf_end, &start_code);
        bytes_left = buf_end - buf;
        switch(start_code) {
        case PICTURE_START_CODE:
            if (bytes_left >= 2) {
                s->pict_type = (buf[1] >> 3) & 7;
                if (bytes_left >= 4)
                vbv_delay = ((buf[1] & 0x07) << 13) | (buf[2] << 5) | (buf[3]  >> 3);
            }
            break;
        case SEQ_START_CODE:
            if (bytes_left >= 7) {
                pc->width  = (buf[0] << 4) | (buf[1] >> 4);
                pc->height = ((buf[1] & 0x0f) << 8) | buf[2];
                if(!avctx->width || !avctx->height || !avctx->coded_width || !avctx->coded_height){
                    set_dim_ret = ff_set_dimensions(avctx, pc->width, pc->height);
                    did_set_size=1;
                }
                pix_fmt = AV_PIX_FMT_YUV420P;
                frame_rate_index = buf[3] & 0xf;
                pc->frame_rate = avctx->framerate = ff_mpeg12_frame_rate_tab[frame_rate_index];
                bit_rate = (buf[4]<<10) | (buf[5]<<2) | (buf[6]>>6);
                avctx->codec_id = AV_CODEC_ID_MPEG1VIDEO;
                avctx->ticks_per_frame = 1;
            }
            break;
        case EXT_START_CODE:
            if (bytes_left >= 1) {
                ext_type = (buf[0] >> 4);
                switch(ext_type) {
                case 0x1: /
                    if (bytes_left >= 6) {
                        horiz_size_ext = ((buf[1] & 1) << 1) | (buf[2] >> 7);
                        vert_size_ext = (buf[2] >> 5) & 3;
                        bit_rate_ext = ((buf[2] & 0x1F)<<7) | (buf[3]>>1);
                        frame_rate_ext_n = (buf[5] >> 5) & 3;
                        frame_rate_ext_d = (buf[5] & 0x1f);
                        pc->progressive_sequence = buf[1] & (1 << 3);
                        avctx->has_b_frames= !(buf[5] >> 7);

                        chroma_format = (buf[1] >> 1) & 3;
                        switch (chroma_format) {
                        case 1: pix_fmt = AV_PIX_FMT_YUV420P; break;
                        case 2: pix_fmt = AV_PIX_FMT_YUV422P; break;
                        case 3: pix_fmt = AV_PIX_FMT_YUV444P; break;
                        }

                        pc->width  = (pc->width & 0xFFF) | (horiz_size_ext << 12);
                        pc->height = (pc->height& 0xFFF) | ( vert_size_ext << 12);
                        bit_rate = (bit_rate&0x3FFFF) | (bit_rate_ext << 18);
                        if(did_set_size)
                            set_dim_ret = ff_set_dimensions(avctx, pc->width, pc->height);
                        avctx->framerate.num = pc->frame_rate.num * (frame_rate_ext_n + 1);
                        avctx->framerate.den = pc->frame_rate.den * (frame_rate_ext_d + 1);
                        avctx->codec_id = AV_CODEC_ID_MPEG2VIDEO;
                        avctx->ticks_per_frame = 2;
                    }
                    break;
                case 0x8: /
                    if (bytes_left >= 5) {
                        top_field_first = buf[3] & (1 << 7);
                        repeat_first_field = buf[3] & (1 << 1);
                        progressive_frame = buf[4] & (1 << 7);

                        /
                        s->repeat_pict = 1;
                        if (repeat_first_field) {
                            if (pc->progressive_sequence) {
                                if (top_field_first)
                                    s->repeat_pict = 5;
                                else
                                    s->repeat_pict = 3;
                            } else if (progressive_frame) {
                                s->repeat_pict = 2;
                            }
                        }

                        if (!pc->progressive_sequence) {
                            if (top_field_first)
                                s->field_order = AV_FIELD_TT;
                            else
                                s->field_order = AV_FIELD_BB;
                        } else
                            s->field_order = AV_FIELD_PROGRESSIVE;
                    }
                    break;
                }
            }
            break;
        case -1:
            goto the_end;
        default:
            /
            if (start_code >= SLICE_MIN_START_CODE &&
                start_code <= SLICE_MAX_START_CODE)
                goto the_end;
            break;
        }
    }
 the_end: ;
    if (set_dim_ret < 0)
        av_log(avctx, AV_LOG_ERROR, "Failed to set dimensions\n");

    if (avctx->codec_id == AV_CODEC_ID_MPEG2VIDEO && bit_rate) {
        avctx->rc_max_rate = 400*bit_rate;
    }
    if (bit_rate &&
        ((avctx->codec_id == AV_CODEC_ID_MPEG1VIDEO && bit_rate != 0x3FFFF) || vbv_delay != 0xFFFF)) {
        avctx->bit_rate = 400*bit_rate;
    }

    if (pix_fmt != AV_PIX_FMT_NONE) {
        s->format = pix_fmt;
        s->width  = s->coded_width  = pc->width;
        s->height = s->coded_height = pc->height;
    }

#if FF_API_AVCTX_TIMEBASE
    if (avctx->framerate.num)
        avctx->time_base = av_inv_q(av_mul_q(avctx->framerate, (AVRational){avctx->ticks_per_frame, 1}));
#endif
}
