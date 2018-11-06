static int bfi_decode_frame(AVCodecContext *avctx, void *data,
                            int *data_size, AVPacket *avpkt)
{
    GetByteContext g;
    int buf_size    = avpkt->size;
    BFIContext *bfi = avctx->priv_data;
    uint8_t *dst    = bfi->dst;
    uint8_t *src, *dst_offset, colour1, colour2;
    uint8_t *frame_end = bfi->dst + avctx->width * avctx->height;
    uint32_t *pal;
    int i, j, height = avctx->height;

    if (bfi->frame.data[0])
        avctx->release_buffer(avctx, &bfi->frame);

    bfi->frame.reference = 3;

    if (avctx->get_buffer(avctx, &bfi->frame) < 0) {
        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");
        return -1;
    }

    bytestream2_init(&g, avpkt->data, buf_size);

    /
    if (!avctx->frame_number) {
        bfi->frame.pict_type = AV_PICTURE_TYPE_I;
        bfi->frame.key_frame = 1;
        /
        if (avctx->extradata_size > 768) {
            av_log(NULL, AV_LOG_ERROR, "Palette is too large.\n");
            return -1;
        }
        pal = (uint32_t *)bfi->frame.data[1];
        for (i = 0; i < avctx->extradata_size / 3; i++) {
            int shift = 16;
            *pal = 0xFF << 24;
            for (j = 0; j < 3; j++, shift -= 8)
                *pal += ((avctx->extradata[i * 3 + j] << 2) |
                         (avctx->extradata[i * 3 + j] >> 4)) << shift;
            pal++;
        }
        memcpy(bfi->pal, bfi->frame.data[1], sizeof(bfi->pal));
        bfi->frame.palette_has_changed = 1;
    } else {
        bfi->frame.pict_type = AV_PICTURE_TYPE_P;
        bfi->frame.key_frame = 0;
        bfi->frame.palette_has_changed = 0;
        memcpy(bfi->frame.data[1], bfi->pal, sizeof(bfi->pal));
    }

    bytestream2_skip(&g, 4); // Unpacked size, not required.

    while (dst != frame_end) {
        static const uint8_t lentab[4] = { 0, 2, 0, 1 };
        unsigned int byte   = bytestream2_get_byte(&g), av_uninit(offset);
        unsigned int code   = byte >> 6;
        unsigned int length = byte & ~0xC0;

        if (!bytestream2_get_bytes_left(&g)) {
            av_log(avctx, AV_LOG_ERROR,
                   "Input resolution larger than actual frame.\n");
            return -1;
        }

        /
        if (length == 0) {
            if (code == 1) {
                length = bytestream2_get_byte(&g);
                offset = bytestream2_get_le16(&g);
            } else {
                length = bytestream2_get_le16(&g);
                if (code == 2 && length == 0)
                    break;
            }
        } else {
            if (code == 1)
                offset = bytestream2_get_byte(&g);
        }

        /
        if (dst + (length << lentab[code]) > frame_end)
            break;

        switch (code) {
        case 0:                // normal chain
            if (length >= bytestream2_get_bytes_left(&g)) {
                av_log(avctx, AV_LOG_ERROR, "Frame larger than buffer.\n");
                return -1;
            }
            bytestream2_get_buffer(&g, dst, length);
            dst += length;
            break;
        case 1:                // back chain
            dst_offset = dst - offset;
            length    *= 4;     // Convert dwords to bytes.
            if (dst_offset < bfi->dst)
                break;
            while (length--)
                *dst++ = *dst_offset++;
            break;
        case 2:                // skip chain
            dst += length;
            break;
        case 3:                // fill chain
            colour1 = bytestream2_get_byte(&g);
            colour2 = bytestream2_get_byte(&g);
            while (length--) {
                *dst++ = colour1;
                *dst++ = colour2;
            }
            break;
        }
    }

    src = bfi->dst;
    dst = bfi->frame.data[0];
    while (height--) {
        memcpy(dst, src, avctx->width);
        src += avctx->width;
        dst += bfi->frame.linesize[0];
    }
    *data_size       = sizeof(AVFrame);
    *(AVFrame *)data = bfi->frame;
    return buf_size;
}
