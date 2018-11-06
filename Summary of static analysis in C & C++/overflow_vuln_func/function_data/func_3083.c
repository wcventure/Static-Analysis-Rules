static int
avs_decode_frame(AVCodecContext * avctx,
                 void *data, int *data_size, AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    const uint8_t *buf_end = avpkt->data + avpkt->size;
    int buf_size = avpkt->size;
    AvsContext *const avs = avctx->priv_data;
    AVFrame *picture = data;
    AVFrame *const p =  &avs->picture;
    const uint8_t *table, *vect;
    uint8_t *out;
    int i, j, x, y, stride, vect_w = 3, vect_h = 3;
    AvsVideoSubType sub_type;
    AvsBlockType type;
    GetBitContext change_map;

    if (avctx->reget_buffer(avctx, p)) {
        av_log(avctx, AV_LOG_ERROR, "reget_buffer() failed\n");
        return -1;
    }
    p->reference = 3;
    p->pict_type = AV_PICTURE_TYPE_P;
    p->key_frame = 0;

    out = avs->picture.data[0];
    stride = avs->picture.linesize[0];

    if (buf_end - buf < 4)
        return AVERROR_INVALIDDATA;
    sub_type = buf[0];
    type = buf[1];
    buf += 4;

    if (type == AVS_PALETTE) {
        int first, last;
        uint32_t *pal = (uint32_t *) avs->picture.data[1];

        first = AV_RL16(buf);
        last = first + AV_RL16(buf + 2);
        if (first >= 256 || last > 256 || buf_end - buf < 4 + 4 + 3 * (last - first))
            return AVERROR_INVALIDDATA;
        buf += 4;
        for (i=first; i<last; i++, buf+=3) {
            pal[i] = (buf[0] << 18) | (buf[1] << 10) | (buf[2] << 2);
            pal[i] |= 0xFF << 24 | (pal[i] >> 6) & 0x30303;
        }

        sub_type = buf[0];
        type = buf[1];
        buf += 4;
    }

    if (type != AVS_VIDEO)
        return -1;

    switch (sub_type) {
    case AVS_I_FRAME:
        p->pict_type = AV_PICTURE_TYPE_I;
        p->key_frame = 1;
    case AVS_P_FRAME_3X3:
        vect_w = 3;
        vect_h = 3;
        break;

    case AVS_P_FRAME_2X2:
        vect_w = 2;
        vect_h = 2;
        break;

    case AVS_P_FRAME_2X3:
        vect_w = 2;
        vect_h = 3;
        break;

    default:
      return -1;
    }

    if (buf_end - buf < 256 * vect_w * vect_h)
        return AVERROR_INVALIDDATA;
    table = buf + (256 * vect_w * vect_h);
    if (sub_type != AVS_I_FRAME) {
        int map_size = ((318 / vect_w + 7) / 8) * (198 / vect_h);
        if (buf_end - table < map_size)
            return AVERROR_INVALIDDATA;
        init_get_bits(&change_map, table, map_size * 8);
        table += map_size;
    }

    for (y=0; y<198; y+=vect_h) {
        for (x=0; x<318; x+=vect_w) {
            if (sub_type == AVS_I_FRAME || get_bits1(&change_map)) {
                if (buf_end - table < 1)
                    return AVERROR_INVALIDDATA;
                vect = &buf[*table++ * (vect_w * vect_h)];
                for (j=0; j<vect_w; j++) {
                    out[(y + 0) * stride + x + j] = vect[(0 * vect_w) + j];
                    out[(y + 1) * stride + x + j] = vect[(1 * vect_w) + j];
                    if (vect_h == 3)
                        out[(y + 2) * stride + x + j] =
                            vect[(2 * vect_w) + j];
                }
            }
        }
        if (sub_type != AVS_I_FRAME)
            align_get_bits(&change_map);
    }

    *picture   = avs->picture;
    *data_size = sizeof(AVPicture);

    return buf_size;
}
