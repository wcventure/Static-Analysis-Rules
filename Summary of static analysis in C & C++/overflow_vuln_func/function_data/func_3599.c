static int cinvideo_decode_frame(AVCodecContext *avctx,
                                 void *data, int *data_size,
                                 AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    CinVideoContext *cin = avctx->priv_data;
    int i, y, palette_type, palette_colors_count, bitmap_frame_type, bitmap_frame_size, res = 0;

    palette_type = buf[0];
    palette_colors_count = AV_RL16(buf+1);
    bitmap_frame_type = buf[3];
    buf += 4;

    bitmap_frame_size = buf_size - 4;

    /
    if (bitmap_frame_size < palette_colors_count * (3 + (palette_type != 0)))
        return AVERROR_INVALIDDATA;
    if (palette_type == 0) {
        if (palette_colors_count > 256)
            return AVERROR_INVALIDDATA;
        for (i = 0; i < palette_colors_count; ++i) {
            cin->palette[i] = 0xFF << 24 | bytestream_get_le24(&buf);
            bitmap_frame_size -= 3;
        }
    } else {
        for (i = 0; i < palette_colors_count; ++i) {
            cin->palette[buf[0]] = 0xFF << 24 | AV_RL24(buf+1);
            buf += 4;
            bitmap_frame_size -= 4;
        }
    }

    /
    switch (bitmap_frame_type) {
    case 9:
        cin_decode_rle(buf, bitmap_frame_size,
          cin->bitmap_table[CIN_CUR_BMP], cin->bitmap_size);
        break;
    case 34:
        cin_decode_rle(buf, bitmap_frame_size,
          cin->bitmap_table[CIN_CUR_BMP], cin->bitmap_size);
        cin_apply_delta_data(cin->bitmap_table[CIN_PRE_BMP],
          cin->bitmap_table[CIN_CUR_BMP], cin->bitmap_size);
        break;
    case 35:
        cin_decode_huffman(buf, bitmap_frame_size,
          cin->bitmap_table[CIN_INT_BMP], cin->bitmap_size);
        cin_decode_rle(cin->bitmap_table[CIN_INT_BMP], bitmap_frame_size,
          cin->bitmap_table[CIN_CUR_BMP], cin->bitmap_size);
        break;
    case 36:
        bitmap_frame_size = cin_decode_huffman(buf, bitmap_frame_size,
          cin->bitmap_table[CIN_INT_BMP], cin->bitmap_size);
        cin_decode_rle(cin->bitmap_table[CIN_INT_BMP], bitmap_frame_size,
          cin->bitmap_table[CIN_CUR_BMP], cin->bitmap_size);
        cin_apply_delta_data(cin->bitmap_table[CIN_PRE_BMP],
          cin->bitmap_table[CIN_CUR_BMP], cin->bitmap_size);
        break;
    case 37:
        cin_decode_huffman(buf, bitmap_frame_size,
          cin->bitmap_table[CIN_CUR_BMP], cin->bitmap_size);
        break;
    case 38:
        res = cin_decode_lzss(buf, bitmap_frame_size,
                              cin->bitmap_table[CIN_CUR_BMP],
                              cin->bitmap_size);
        if (res < 0)
            return res;
        break;
    case 39:
        res = cin_decode_lzss(buf, bitmap_frame_size,
                              cin->bitmap_table[CIN_CUR_BMP],
                              cin->bitmap_size);
        if (res < 0)
            return res;
        cin_apply_delta_data(cin->bitmap_table[CIN_PRE_BMP],
          cin->bitmap_table[CIN_CUR_BMP], cin->bitmap_size);
        break;
    }

    cin->frame.buffer_hints = FF_BUFFER_HINTS_VALID | FF_BUFFER_HINTS_PRESERVE | FF_BUFFER_HINTS_REUSABLE;
    if (avctx->reget_buffer(avctx, &cin->frame)) {
        av_log(cin->avctx, AV_LOG_ERROR, "delphinecinvideo: reget_buffer() failed to allocate a frame\n");
        return -1;
    }

    memcpy(cin->frame.data[1], cin->palette, sizeof(cin->palette));
    cin->frame.palette_has_changed = 1;
    for (y = 0; y < cin->avctx->height; ++y)
        memcpy(cin->frame.data[0] + (cin->avctx->height - 1 - y) * cin->frame.linesize[0],
          cin->bitmap_table[CIN_CUR_BMP] + y * cin->avctx->width,
          cin->avctx->width);

    FFSWAP(uint8_t *, cin->bitmap_table[CIN_CUR_BMP], cin->bitmap_table[CIN_PRE_BMP]);

    *data_size = sizeof(AVFrame);
    *(AVFrame *)data = cin->frame;

    return buf_size;
}
