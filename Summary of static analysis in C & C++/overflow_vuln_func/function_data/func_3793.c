static int flic_decode_frame_8BPP(AVCodecContext *avctx,
                                  void *data, int *data_size,
                                  const uint8_t *buf, int buf_size)
{
    FlicDecodeContext *s = avctx->priv_data;

    int stream_ptr = 0;
    int stream_ptr_after_color_chunk;
    int pixel_ptr;
    int palette_ptr;
    unsigned char palette_idx1;
    unsigned char palette_idx2;

    unsigned int frame_size;
    int num_chunks;

    unsigned int chunk_size;
    int chunk_type;

    int i, j;

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

    s->frame.reference = 1;
    s->frame.buffer_hints = FF_BUFFER_HINTS_VALID | FF_BUFFER_HINTS_PRESERVE | FF_BUFFER_HINTS_REUSABLE;
    if (avctx->reget_buffer(avctx, &s->frame) < 0) {
        av_log(avctx, AV_LOG_ERROR, "reget_buffer() failed\n");
        return -1;
    }

    pixels = s->frame.data[0];
    pixel_limit = s->avctx->height * s->frame.linesize[0];

    frame_size = AV_RL32(&buf[stream_ptr]);
    stream_ptr += 6;  /
    num_chunks = AV_RL16(&buf[stream_ptr]);
    stream_ptr += 10;  /

    frame_size -= 16;

    /
    while ((frame_size > 0) && (num_chunks > 0)) {
        chunk_size = AV_RL32(&buf[stream_ptr]);
        if (chunk_size > frame_size) {
            av_log(avctx, AV_LOG_WARNING,
                   "Invalid chunk_size = %u > frame_size = %u\n", chunk_size, frame_size);
            chunk_size = frame_size;
        }
        stream_ptr += 4;
        chunk_type = AV_RL16(&buf[stream_ptr]);
        stream_ptr += 2;

        switch (chunk_type) {
        case FLI_256_COLOR:
        case FLI_COLOR:
            stream_ptr_after_color_chunk = stream_ptr + chunk_size - 6;

            /
            if ((chunk_type == FLI_256_COLOR) && (s->fli_type != FLC_MAGIC_CARPET_SYNTHETIC_TYPE_CODE))
                color_shift = 0;
            else
                color_shift = 2;
            /
            color_packets = AV_RL16(&buf[stream_ptr]);
            stream_ptr += 2;
            palette_ptr = 0;
            for (i = 0; i < color_packets; i++) {
                /
                palette_ptr += buf[stream_ptr++];

                /
                color_changes = buf[stream_ptr++];

                /
                if (color_changes == 0)
                    color_changes = 256;

                for (j = 0; j < color_changes; j++) {
                    unsigned int entry;

                    /
                    if ((unsigned)palette_ptr >= 256)
                        palette_ptr = 0;

                    r = buf[stream_ptr++] << color_shift;
                    g = buf[stream_ptr++] << color_shift;
                    b = buf[stream_ptr++] << color_shift;
                    entry = (r << 16) | (g << 8) | b;
                    if (s->palette[palette_ptr] != entry)
                        s->new_palette = 1;
                    s->palette[palette_ptr++] = entry;
                }
            }

            /
            stream_ptr = stream_ptr_after_color_chunk;

            break;

        case FLI_DELTA:
            y_ptr = 0;
            compressed_lines = AV_RL16(&buf[stream_ptr]);
            stream_ptr += 2;
            while (compressed_lines > 0) {
                line_packets = AV_RL16(&buf[stream_ptr]);
                stream_ptr += 2;
                if ((line_packets & 0xC000) == 0xC000) {
                    // line skip opcode
                    line_packets = -line_packets;
                    y_ptr += line_packets * s->frame.linesize[0];
                } else if ((line_packets & 0xC000) == 0x4000) {
                    av_log(avctx, AV_LOG_ERROR, "Undefined opcode (%x) in DELTA_FLI\n", line_packets);
                } else if ((line_packets & 0xC000) == 0x8000) {
                    // "last byte" opcode
                    pixel_ptr= y_ptr + s->frame.linesize[0] - 1;
                    CHECK_PIXEL_PTR(0);
                    pixels[pixel_ptr] = line_packets & 0xff;
                } else {
                    compressed_lines--;
                    pixel_ptr = y_ptr;
                    CHECK_PIXEL_PTR(0);
                    pixel_countdown = s->avctx->width;
                    for (i = 0; i < line_packets; i++) {
                        /
                        pixel_skip = buf[stream_ptr++];
                        pixel_ptr += pixel_skip;
                        pixel_countdown -= pixel_skip;
                        byte_run = (signed char)(buf[stream_ptr++]);
                        if (byte_run < 0) {
                            byte_run = -byte_run;
                            palette_idx1 = buf[stream_ptr++];
                            palette_idx2 = buf[stream_ptr++];
                            CHECK_PIXEL_PTR(byte_run * 2);
                            for (j = 0; j < byte_run; j++, pixel_countdown -= 2) {
                                pixels[pixel_ptr++] = palette_idx1;
                                pixels[pixel_ptr++] = palette_idx2;
                            }
                        } else {
                            CHECK_PIXEL_PTR(byte_run * 2);
                            for (j = 0; j < byte_run * 2; j++, pixel_countdown--) {
                                palette_idx1 = buf[stream_ptr++];
                                pixels[pixel_ptr++] = palette_idx1;
                            }
                        }
                    }

                    y_ptr += s->frame.linesize[0];
                }
            }
            break;

        case FLI_LC:
            /
            starting_line = AV_RL16(&buf[stream_ptr]);
            stream_ptr += 2;
            y_ptr = 0;
            y_ptr += starting_line * s->frame.linesize[0];

            compressed_lines = AV_RL16(&buf[stream_ptr]);
            stream_ptr += 2;
            while (compressed_lines > 0) {
                pixel_ptr = y_ptr;
                CHECK_PIXEL_PTR(0);
                pixel_countdown = s->avctx->width;
                line_packets = buf[stream_ptr++];
                if (line_packets > 0) {
                    for (i = 0; i < line_packets; i++) {
                        /
                        pixel_skip = buf[stream_ptr++];
                        pixel_ptr += pixel_skip;
                        pixel_countdown -= pixel_skip;
                        byte_run = (signed char)(buf[stream_ptr++]);
                        if (byte_run > 0) {
                            CHECK_PIXEL_PTR(byte_run);
                            for (j = 0; j < byte_run; j++, pixel_countdown--) {
                                palette_idx1 = buf[stream_ptr++];
                                pixels[pixel_ptr++] = palette_idx1;
                            }
                        } else if (byte_run < 0) {
                            byte_run = -byte_run;
                            palette_idx1 = buf[stream_ptr++];
                            CHECK_PIXEL_PTR(byte_run);
                            for (j = 0; j < byte_run; j++, pixel_countdown--) {
                                pixels[pixel_ptr++] = palette_idx1;
                            }
                        }
                    }
                }

                y_ptr += s->frame.linesize[0];
                compressed_lines--;
            }
            break;

        case FLI_BLACK:
            /
            memset(pixels, 0,
                s->frame.linesize[0] * s->avctx->height);
            break;

        case FLI_BRUN:
            /
            y_ptr = 0;
            for (lines = 0; lines < s->avctx->height; lines++) {
                pixel_ptr = y_ptr;
                /
                stream_ptr++;
                pixel_countdown = s->avctx->width;
                while (pixel_countdown > 0) {
                    byte_run = (signed char)(buf[stream_ptr++]);
                    if (byte_run > 0) {
                        palette_idx1 = buf[stream_ptr++];
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
                        for (j = 0; j < byte_run; j++) {
                            palette_idx1 = buf[stream_ptr++];
                            pixels[pixel_ptr++] = palette_idx1;
                            pixel_countdown--;
                            if (pixel_countdown < 0)
                                av_log(avctx, AV_LOG_ERROR, "pixel_countdown < 0 (%d) at line %d\n",
                                       pixel_countdown, lines);
                        }
                    }
                }

                y_ptr += s->frame.linesize[0];
            }
            break;

        case FLI_COPY:
            /
            if (chunk_size - 6 > s->avctx->width * s->avctx->height) {
                av_log(avctx, AV_LOG_ERROR, "In chunk FLI_COPY : source data (%d bytes) " \
                       "bigger than image, skipping chunk\n", chunk_size - 6);
                stream_ptr += chunk_size - 6;
            } else {
                for (y_ptr = 0; y_ptr < s->frame.linesize[0] * s->avctx->height;
                     y_ptr += s->frame.linesize[0]) {
                    memcpy(&pixels[y_ptr], &buf[stream_ptr],
                        s->avctx->width);
                    stream_ptr += s->avctx->width;
                }
            }
            break;

        case FLI_MINI:
            /
            stream_ptr += chunk_size - 6;
            break;

        default:
            av_log(avctx, AV_LOG_ERROR, "Unrecognized chunk type: %d\n", chunk_type);
            break;
        }

        frame_size -= chunk_size;
        num_chunks--;
    }

    /
    if ((stream_ptr != buf_size) && (stream_ptr != buf_size - 1))
        av_log(avctx, AV_LOG_ERROR, "Processed FLI chunk where chunk size = %d " \
               "and final chunk ptr = %d\n", buf_size, stream_ptr);

    /
    memcpy(s->frame.data[1], s->palette, AVPALETTE_SIZE);
    if (s->new_palette) {
        s->frame.palette_has_changed = 1;
        s->new_palette = 0;
    }

    *data_size=sizeof(AVFrame);
    *(AVFrame*)data = s->frame;

    return buf_size;
}
