static int flic_decode_frame_8BPP(AVCodecContext *avctx,
                                  void *data, int *got_frame,
                                  const uint8_t *buf, int buf_size)
{
    FlicDecodeContext *s = avctx->priv_data;

    GetByteContext g2;
    int pixel_ptr;
    int palette_ptr;
    unsigned char palette_idx1;
    unsigned char palette_idx2;

    unsigned int frame_size;
    int num_chunks;

    unsigned int chunk_size;
    int chunk_type;

    int i, j, ret;

    int color_packets;
    int color_changes;
    int color_shift;
    unsigned char r, g, b;

    int lines;
    int compressed_lines;
    int starting_line;
    signed short line_packets;
    int y_ptr;
    int byte_run;
    int pixel_skip;
    int pixel_countdown;
    unsigned char *pixels;
    unsigned int pixel_limit;

    bytestream2_init(&g2, buf, buf_size);

    if ((ret = ff_reget_buffer(avctx, s->frame)) < 0)
        return ret;

    pixels = s->frame->data[0];
    pixel_limit = s->avctx->height * s->frame->linesize[0];
    if (buf_size < 16 || buf_size > INT_MAX - (3 * 256 + AV_INPUT_BUFFER_PADDING_SIZE))
        return AVERROR_INVALIDDATA;
    frame_size = bytestream2_get_le32(&g2);
    if (frame_size > buf_size)
        frame_size = buf_size;
    bytestream2_skip(&g2, 2); /
    num_chunks = bytestream2_get_le16(&g2);
    bytestream2_skip(&g2, 8);  /

    frame_size -= 16;

    /
    while ((frame_size >= 6) && (num_chunks > 0) &&
            bytestream2_get_bytes_left(&g2) >= 4) {
        int stream_ptr_after_chunk;
        chunk_size = bytestream2_get_le32(&g2);
        if (chunk_size > frame_size) {
            av_log(avctx, AV_LOG_WARNING,
                   "Invalid chunk_size = %u > frame_size = %u\n", chunk_size, frame_size);
            chunk_size = frame_size;
        }
        stream_ptr_after_chunk = bytestream2_tell(&g2) - 4 + chunk_size;

        chunk_type = bytestream2_get_le16(&g2);

        switch (chunk_type) {
        case FLI_256_COLOR:
        case FLI_COLOR:
            /
            if ((chunk_type == FLI_256_COLOR) && (s->fli_type != FLC_MAGIC_CARPET_SYNTHETIC_TYPE_CODE))
                color_shift = 0;
            else
                color_shift = 2;
            /
            color_packets = bytestream2_get_le16(&g2);
            palette_ptr = 0;
            for (i = 0; i < color_packets; i++) {
                /
                palette_ptr += bytestream2_get_byte(&g2);

                /
                color_changes = bytestream2_get_byte(&g2);

                /
                if (color_changes == 0)
                    color_changes = 256;

                if (bytestream2_tell(&g2) + color_changes * 3 > stream_ptr_after_chunk)
                    break;

                for (j = 0; j < color_changes; j++) {
                    unsigned int entry;

                    /
                    if ((unsigned)palette_ptr >= 256)
                        palette_ptr = 0;

                    r = bytestream2_get_byte(&g2) << color_shift;
                    g = bytestream2_get_byte(&g2) << color_shift;
                    b = bytestream2_get_byte(&g2) << color_shift;
                    entry = 0xFFU << 24 | r << 16 | g << 8 | b;
                    if (color_shift == 2)
                        entry |= entry >> 6 & 0x30303;
                    if (s->palette[palette_ptr] != entry)
                        s->new_palette = 1;
                    s->palette[palette_ptr++] = entry;
                }
            }
            break;

        case FLI_DELTA:
            y_ptr = 0;
            compressed_lines = bytestream2_get_le16(&g2);
            while (compressed_lines > 0) {
                if (bytestream2_tell(&g2) + 2 > stream_ptr_after_chunk)
                    break;
                line_packets = bytestream2_get_le16(&g2);
                if ((line_packets & 0xC000) == 0xC000) {
                    // line skip opcode
                    line_packets = -line_packets;
                    y_ptr += line_packets * s->frame->linesize[0];
                } else if ((line_packets & 0xC000) == 0x4000) {
                    av_log(avctx, AV_LOG_ERROR, "Undefined opcode (%x) in DELTA_FLI\n", line_packets);
                } else if ((line_packets & 0xC000) == 0x8000) {
                    // "last byte" opcode
                    pixel_ptr= y_ptr + s->frame->linesize[0] - 1;
                    CHECK_PIXEL_PTR(0);
                    pixels[pixel_ptr] = line_packets & 0xff;
                } else {
                    compressed_lines--;
                    pixel_ptr = y_ptr;
                    CHECK_PIXEL_PTR(0);
                    pixel_countdown = s->avctx->width;
                    for (i = 0; i < line_packets; i++) {
                        if (bytestream2_tell(&g2) + 2 > stream_ptr_after_chunk)
                            break;
                        /
                        pixel_skip = bytestream2_get_byte(&g2);
                        pixel_ptr += pixel_skip;
                        pixel_countdown -= pixel_skip;
                        byte_run = sign_extend(bytestream2_get_byte(&g2), 8);
                        if (byte_run < 0) {
                            byte_run = -byte_run;
                            palette_idx1 = bytestream2_get_byte(&g2);
                            palette_idx2 = bytestream2_get_byte(&g2);
                            CHECK_PIXEL_PTR(byte_run * 2);
                            for (j = 0; j < byte_run; j++, pixel_countdown -= 2) {
                                pixels[pixel_ptr++] = palette_idx1;
                                pixels[pixel_ptr++] = palette_idx2;
                            }
                        } else {
                            CHECK_PIXEL_PTR(byte_run * 2);
                            if (bytestream2_tell(&g2) + byte_run * 2 > stream_ptr_after_chunk)
                                break;
                            for (j = 0; j < byte_run * 2; j++, pixel_countdown--) {
                                pixels[pixel_ptr++] = bytestream2_get_byte(&g2);
                            }
                        }
                    }

                    y_ptr += s->frame->linesize[0];
                }
            }
            break;

        case FLI_LC:
            /
            starting_line = bytestream2_get_le16(&g2);
            y_ptr = 0;
            y_ptr += starting_line * s->frame->linesize[0];

            compressed_lines = bytestream2_get_le16(&g2);
            while (compressed_lines > 0) {
                pixel_ptr = y_ptr;
                CHECK_PIXEL_PTR(0);
                pixel_countdown = s->avctx->width;
                if (bytestream2_tell(&g2) + 1 > stream_ptr_after_chunk)
                    break;
                line_packets = bytestream2_get_byte(&g2);
                if (line_packets > 0) {
                    for (i = 0; i < line_packets; i++) {
                        /
                        if (bytestream2_tell(&g2) + 1 > stream_ptr_after_chunk)
                            break;
                        pixel_skip = bytestream2_get_byte(&g2);
                        pixel_ptr += pixel_skip;
                        pixel_countdown -= pixel_skip;
                        byte_run = sign_extend(bytestream2_get_byte(&g2),8);
                        if (byte_run > 0) {
                            CHECK_PIXEL_PTR(byte_run);
                            if (bytestream2_tell(&g2) + byte_run > stream_ptr_after_chunk)
                                break;
                            for (j = 0; j < byte_run; j++, pixel_countdown--) {
                                pixels[pixel_ptr++] = bytestream2_get_byte(&g2);
                            }
                        } else if (byte_run < 0) {
                            byte_run = -byte_run;
                            palette_idx1 = bytestream2_get_byte(&g2);
                            CHECK_PIXEL_PTR(byte_run);
                            for (j = 0; j < byte_run; j++, pixel_countdown--) {
                                pixels[pixel_ptr++] = palette_idx1;
                            }
                        }
                    }
                }

                y_ptr += s->frame->linesize[0];
                compressed_lines--;
            }
            break;

        case FLI_BLACK:
            /
            memset(pixels, 0,
                s->frame->linesize[0] * s->avctx->height);
            break;

        case FLI_BRUN:
            /
            y_ptr = 0;
            for (lines = 0; lines < s->avctx->height; lines++) {
                pixel_ptr = y_ptr;
                /
                 bytestream2_skip(&g2, 1);
                pixel_countdown = s->avctx->width;
                while (pixel_countdown > 0) {
                    if (bytestream2_tell(&g2) + 1 > stream_ptr_after_chunk)
                        break;
                    byte_run = sign_extend(bytestream2_get_byte(&g2), 8);
                    if (!byte_run) {
                        av_log(avctx, AV_LOG_ERROR, "Invalid byte run value.\n");
                        return AVERROR_INVALIDDATA;
                    }

                    if (byte_run > 0) {
                        palette_idx1 = bytestream2_get_byte(&g2);
                        CHECK_PIXEL_PTR(byte_run);
                        for (j = 0; j < byte_run; j++) {
                            pixels[pixel_ptr++] = palette_idx1;
                            pixel_countdown--;
                            if (pixel_countdown < 0)
                                av_log(avctx, AV_LOG_ERROR, "pixel_countdown < 0 (%d) at line %d\n",
                                       pixel_countdown, lines);
                        }
                    } else {  /
                        byte_run = -byte_run;
                        CHECK_PIXEL_PTR(byte_run);
                        if (bytestream2_tell(&g2) + byte_run > stream_ptr_after_chunk)
                            break;
                        for (j = 0; j < byte_run; j++) {
                            pixels[pixel_ptr++] = bytestream2_get_byte(&g2);
                            pixel_countdown--;
                            if (pixel_countdown < 0)
                                av_log(avctx, AV_LOG_ERROR, "pixel_countdown < 0 (%d) at line %d\n",
                                       pixel_countdown, lines);
                        }
                    }
                }

                y_ptr += s->frame->linesize[0];
            }
            break;

        case FLI_COPY:
            /
            if (chunk_size - 6 != FFALIGN(s->avctx->width, 4) * s->avctx->height) {
                av_log(avctx, AV_LOG_ERROR, "In chunk FLI_COPY : source data (%d bytes) " \
                       "has incorrect size, skipping chunk\n", chunk_size - 6);
                bytestream2_skip(&g2, chunk_size - 6);
            } else {
                for (y_ptr = 0; y_ptr < s->frame->linesize[0] * s->avctx->height;
                     y_ptr += s->frame->linesize[0]) {
                    bytestream2_get_buffer(&g2, &pixels[y_ptr],
                                           s->avctx->width);
                    if (s->avctx->width & 3)
                        bytestream2_skip(&g2, 4 - (s->avctx->width & 3));
                }
            }
            break;

        case FLI_MINI:
            /
            break;

        default:
            av_log(avctx, AV_LOG_ERROR, "Unrecognized chunk type: %d\n", chunk_type);
            break;
        }

        if (stream_ptr_after_chunk - bytestream2_tell(&g2) > 0)
            bytestream2_skip(&g2, stream_ptr_after_chunk - bytestream2_tell(&g2));

        frame_size -= chunk_size;
        num_chunks--;
    }

    /
    if (bytestream2_get_bytes_left(&g2) > 2)
        av_log(avctx, AV_LOG_ERROR, "Processed FLI chunk where chunk size = %d " \
               "and final chunk ptr = %d\n", buf_size,
               buf_size - bytestream2_get_bytes_left(&g2));

    /
    memcpy(s->frame->data[1], s->palette, AVPALETTE_SIZE);
    if (s->new_palette) {
        s->frame->palette_has_changed = 1;
        s->new_palette = 0;
    }

    if ((ret = av_frame_ref(data, s->frame)) < 0)
        return ret;

    *got_frame = 1;

    return buf_size;
}
