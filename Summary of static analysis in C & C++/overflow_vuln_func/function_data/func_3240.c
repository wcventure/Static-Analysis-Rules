static int txd_decode_frame(AVCodecContext *avctx, void *data, int *data_size,
                            AVPacket *avpkt) {
    const uint8_t *buf = avpkt->data;
    TXDContext * const s = avctx->priv_data;
    AVFrame *picture = data;
    AVFrame * const p = &s->picture;
    unsigned int version, w, h, d3d_format, depth, stride, mipmap_count, flags;
    unsigned int y, v;
    uint8_t *ptr;
    const uint8_t *cur = buf;
    const uint32_t *palette = (const uint32_t *)(cur + 88);
    uint32_t *pal;

    version         = AV_RL32(cur);
    d3d_format      = AV_RL32(cur+76);
    w               = AV_RL16(cur+80);
    h               = AV_RL16(cur+82);
    depth           = AV_RL8 (cur+84);
    mipmap_count    = AV_RL8 (cur+85);
    flags           = AV_RL8 (cur+87);
    cur            += 92;

    if (version < 8 || version > 9) {
        av_log(avctx, AV_LOG_ERROR, "texture data version %i is unsupported\n",
                                                                    version);
        return -1;
    }

    if (depth == 8) {
        avctx->pix_fmt = PIX_FMT_PAL8;
        cur += 1024;
    } else if (depth == 16 || depth == 32)
        avctx->pix_fmt = PIX_FMT_RGB32;
    else {
        av_log(avctx, AV_LOG_ERROR, "depth of %i is unsupported\n", depth);
        return -1;
    }

    if (p->data[0])
        avctx->release_buffer(avctx, p);

    if (av_image_check_size(w, h, 0, avctx))
        return -1;
    if (w != avctx->width || h != avctx->height)
        avcodec_set_dimensions(avctx, w, h);
    if (avctx->get_buffer(avctx, p) < 0) {
        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");
        return -1;
    }

    p->pict_type = AV_PICTURE_TYPE_I;

    ptr    = p->data[0];
    stride = p->linesize[0];

    if (depth == 8) {
        pal = (uint32_t *) p->data[1];
        for (y=0; y<256; y++) {
            v = AV_RB32(palette+y);
            pal[y] = (v>>8) + (v<<24);
        }
        for (y=0; y<h; y++) {
            memcpy(ptr, cur, w);
            ptr += stride;
            cur += w;
        }
    } else if (depth == 16) {
        switch (d3d_format) {
        case 0:
            if (!flags&1) goto unsupported;
        case FF_S3TC_DXT1:
            ff_decode_dxt1(cur, ptr, w, h, stride);
            break;
        case FF_S3TC_DXT3:
            ff_decode_dxt3(cur, ptr, w, h, stride);
            break;
        default:
            goto unsupported;
        }
    } else if (depth == 32) {
        switch (d3d_format) {
        case 0x15:
        case 0x16:
            for (y=0; y<h; y++) {
                memcpy(ptr, cur, w*4);
                ptr += stride;
                cur += w*4;
            }
            break;
        default:
            goto unsupported;
        }
    }

    for (; mipmap_count > 1; mipmap_count--)
        cur += AV_RL32(cur) + 4;

    *picture   = s->picture;
    *data_size = sizeof(AVPicture);

    return cur - buf;

unsupported:
    av_log(avctx, AV_LOG_ERROR, "unsupported d3d format (%08x)\n", d3d_format);
    return -1;
}
