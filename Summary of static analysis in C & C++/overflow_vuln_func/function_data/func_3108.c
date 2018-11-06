static int yop_decode_frame(AVCodecContext *avctx, void *data, int *data_size,
                            AVPacket *avpkt)
{
    YopDecContext *s = avctx->priv_data;
    int tag, firstcolor, is_odd_frame;
    int ret, i;
    uint32_t *palette;

    if (s->frame.data[0])
        avctx->release_buffer(avctx, &s->frame);

    if (avpkt->size < 4 + 3*s->num_pal_colors) {
        av_log(avctx, AV_LOG_ERROR, "packet of size %d too small\n", avpkt->size);
        return AVERROR_INVALIDDATA;
    }

    ret = avctx->get_buffer(avctx, &s->frame);
    if (ret < 0) {
        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");
        return ret;
    }

    s->frame.linesize[0] = avctx->width;

    s->dstbuf     = s->frame.data[0];
    s->dstptr     = s->frame.data[0];
    s->srcptr     = avpkt->data + 4;
    s->row_pos    = 0;
    s->low_nibble = NULL;

    is_odd_frame = avpkt->data[0];
    if(is_odd_frame>1){
        av_log(avctx, AV_LOG_ERROR, "frame is too odd %d\n", is_odd_frame);
        return AVERROR_INVALIDDATA;
    }
    firstcolor   = s->first_color[is_odd_frame];
    palette      = (uint32_t *)s->frame.data[1];

    for (i = 0; i < s->num_pal_colors; i++, s->srcptr += 3) {
        palette[i + firstcolor] = (s->srcptr[0] << 18) |
                                  (s->srcptr[1] << 10) |
                                  (s->srcptr[2] << 2);
        palette[i + firstcolor] |= 0xFF << 24 |
                                   (palette[i + firstcolor] >> 6) & 0x30303;
    }

    s->frame.palette_has_changed = 1;

    while (s->dstptr - s->dstbuf <
           avctx->width * avctx->height &&
           s->srcptr - avpkt->data < avpkt->size) {

        tag = yop_get_next_nibble(s);

        if (tag != 0xf) {
            yop_paint_block(s, tag);
        }else {
            tag = yop_get_next_nibble(s);
            ret = yop_copy_previous_block(s, tag);
            if (ret < 0) {
                avctx->release_buffer(avctx, &s->frame);
                return ret;
            }
        }
        yop_next_macroblock(s);
    }

    *data_size        = sizeof(AVFrame);
    *(AVFrame *) data = s->frame;
    return avpkt->size;
}
