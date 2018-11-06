static int decode_frame(AVCodecContext *avctx, void *data, int *got_frame,
                        AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    UtvideoContext *c = avctx->priv_data;
    int i, j;
    const uint8_t *plane_start[5];
    int plane_size, max_slice_size = 0, slice_start, slice_end, slice_size;
    int ret;
    GetByteContext gb;
    ThreadFrame frame = { .f = data };

    if ((ret = ff_thread_get_buffer(avctx, &frame, 0)) < 0)
        return ret;

    /
    bytestream2_init(&gb, buf, buf_size);
    for (i = 0; i < c->planes; i++) {
        plane_start[i] = gb.buffer;
        if (bytestream2_get_bytes_left(&gb) < 256 + 4 * c->slices) {
            av_log(avctx, AV_LOG_ERROR, "Insufficient data for a plane\n");
            return AVERROR_INVALIDDATA;
        }
        bytestream2_skipu(&gb, 256);
        slice_start = 0;
        slice_end   = 0;
        for (j = 0; j < c->slices; j++) {
            slice_end   = bytestream2_get_le32u(&gb);
            slice_size  = slice_end - slice_start;
            if (slice_end < 0 || slice_size < 0 ||
                bytestream2_get_bytes_left(&gb) < slice_end) {
                av_log(avctx, AV_LOG_ERROR, "Incorrect slice size\n");
                return AVERROR_INVALIDDATA;
            }
            slice_start = slice_end;
            max_slice_size = FFMAX(max_slice_size, slice_size);
        }
        plane_size = slice_end;
        bytestream2_skipu(&gb, plane_size);
    }
    plane_start[c->planes] = gb.buffer;
    if (bytestream2_get_bytes_left(&gb) < c->frame_info_size) {
        av_log(avctx, AV_LOG_ERROR, "Not enough data for frame information\n");
        return AVERROR_INVALIDDATA;
    }
    c->frame_info = bytestream2_get_le32u(&gb);
    av_log(avctx, AV_LOG_DEBUG, "frame information flags %"PRIX32"\n",
           c->frame_info);

    c->frame_pred = (c->frame_info >> 8) & 3;

    if (c->frame_pred == PRED_GRADIENT) {
        avpriv_request_sample(avctx, "Frame with gradient prediction");
        return AVERROR_PATCHWELCOME;
    }

    av_fast_malloc(&c->slice_bits, &c->slice_bits_size,
                   max_slice_size + AV_INPUT_BUFFER_PADDING_SIZE);

    if (!c->slice_bits) {
        av_log(avctx, AV_LOG_ERROR, "Cannot allocate temporary buffer\n");
        return AVERROR(ENOMEM);
    }

    switch (c->avctx->pix_fmt) {
    case AV_PIX_FMT_RGB24:
    case AV_PIX_FMT_RGBA:
        for (i = 0; i < c->planes; i++) {
            ret = decode_plane(c, i, frame.f->data[0] + ff_ut_rgb_order[i],
                               c->planes, frame.f->linesize[0], avctx->width,
                               avctx->height, plane_start[i],
                               c->frame_pred == PRED_LEFT);
            if (ret)
                return ret;
            if (c->frame_pred == PRED_MEDIAN) {
                if (!c->interlaced) {
                    restore_median(frame.f->data[0] + ff_ut_rgb_order[i],
                                   c->planes, frame.f->linesize[0], avctx->width,
                                   avctx->height, c->slices, 0);
                } else {
                    restore_median_il(frame.f->data[0] + ff_ut_rgb_order[i],
                                      c->planes, frame.f->linesize[0],
                                      avctx->width, avctx->height, c->slices,
                                      0);
                }
            }
        }
        restore_rgb_planes(frame.f->data[0], c->planes, frame.f->linesize[0],
                           avctx->width, avctx->height);
        break;
    case AV_PIX_FMT_YUV420P:
        for (i = 0; i < 3; i++) {
            ret = decode_plane(c, i, frame.f->data[i], 1, frame.f->linesize[i],
                               avctx->width >> !!i, avctx->height >> !!i,
                               plane_start[i], c->frame_pred == PRED_LEFT);
            if (ret)
                return ret;
            if (c->frame_pred == PRED_MEDIAN) {
                if (!c->interlaced) {
                    restore_median(frame.f->data[i], 1, frame.f->linesize[i],
                                   avctx->width >> !!i, avctx->height >> !!i,
                                   c->slices, !i);
                } else {
                    restore_median_il(frame.f->data[i], 1, frame.f->linesize[i],
                                      avctx->width  >> !!i,
                                      avctx->height >> !!i,
                                      c->slices, !i);
                }
            }
        }
        break;
    case AV_PIX_FMT_YUV422P:
        for (i = 0; i < 3; i++) {
            ret = decode_plane(c, i, frame.f->data[i], 1, frame.f->linesize[i],
                               avctx->width >> !!i, avctx->height,
                               plane_start[i], c->frame_pred == PRED_LEFT);
            if (ret)
                return ret;
            if (c->frame_pred == PRED_MEDIAN) {
                if (!c->interlaced) {
                    restore_median(frame.f->data[i], 1, frame.f->linesize[i],
                                   avctx->width >> !!i, avctx->height,
                                   c->slices, 0);
                } else {
                    restore_median_il(frame.f->data[i], 1, frame.f->linesize[i],
                                      avctx->width >> !!i, avctx->height,
                                      c->slices, 0);
                }
            }
        }
        break;
    }

    frame.f->key_frame = 1;
    frame.f->pict_type = AV_PICTURE_TYPE_I;
    frame.f->interlaced_frame = !!c->interlaced;

    *got_frame = 1;

    /
    return buf_size;
}
