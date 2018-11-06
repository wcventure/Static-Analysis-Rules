static int decode_frame(AVCodecContext *avctx, void *data,
                        int *got_frame_ptr, AVPacket *pkt)
{
    SANMVideoContext *ctx = avctx->priv_data;
    int i, ret;

    bytestream2_init(&ctx->gb, pkt->data, pkt->size);
    if (ctx->output->data[0])
        avctx->release_buffer(avctx, ctx->output);

    if (!ctx->version) {
        int to_store = 0;

        while (bytestream2_get_bytes_left(&ctx->gb) >= 8) {
            uint32_t sig, size;
            int pos;

            sig  = bytestream2_get_be32u(&ctx->gb);
            size = bytestream2_get_be32u(&ctx->gb);
            pos  = bytestream2_tell(&ctx->gb);

            if (bytestream2_get_bytes_left(&ctx->gb) < size) {
                av_log(avctx, AV_LOG_ERROR, "incorrect chunk size %d\n", size);
                break;
            }
            switch (sig) {
            case MKBETAG('N', 'P', 'A', 'L'):
                if (size != 256 * 3) {
                    av_log(avctx, AV_LOG_ERROR, "incorrect palette block size %d\n",
                           size);
                    return AVERROR_INVALIDDATA;
                }
                for (i = 0; i < 256; i++)
                    ctx->pal[i] = 0xFF << 24 | bytestream2_get_be24u(&ctx->gb);
                break;
            case MKBETAG('F', 'O', 'B', 'J'):
                if (size < 16)
                    return AVERROR_INVALIDDATA;
                if (ret = process_frame_obj(ctx))
                    return ret;
                break;
            case MKBETAG('X', 'P', 'A', 'L'):
                if (size == 6 || size == 4) {
                    uint8_t tmp[3];
                    int j;

                    for (i = 0; i < 256; i++) {
                        for (j = 0; j < 3; j++) {
                            int t = (ctx->pal[i] >> (16 - j * 8)) & 0xFF;
                            tmp[j] = av_clip_uint8((t * 129 + ctx->delta_pal[i * 3 + j]) >> 7);
                        }
                        ctx->pal[i] = 0xFF << 24 | AV_RB24(tmp);
                    }
                } else {
                    if (size < 768 * 2 + 4) {
                        av_log(avctx, AV_LOG_ERROR, "incorrect palette change block size %d\n",
                               size);
                        return AVERROR_INVALIDDATA;
                    }
                    bytestream2_skipu(&ctx->gb, 4);
                    for (i = 0; i < 768; i++)
                        ctx->delta_pal[i] = bytestream2_get_le16u(&ctx->gb);
                    if (size >= 768 * 5 + 4) {
                        for (i = 0; i < 256; i++)
                            ctx->pal[i] = 0xFF << 24 | bytestream2_get_be24u(&ctx->gb);
                    } else {
                        memset(ctx->pal, 0, sizeof(ctx->pal));
                    }
                }
                break;
            case MKBETAG('S', 'T', 'O', 'R'):
                to_store = 1;
                break;
            case MKBETAG('F', 'T', 'C', 'H'):
                memcpy(ctx->frm0, ctx->stored_frame, ctx->buf_size);
                break;
            default:
                bytestream2_skip(&ctx->gb, size);
                av_log(avctx, AV_LOG_DEBUG, "unknown/unsupported chunk %x\n", sig);
                break;
            }

            bytestream2_seek(&ctx->gb, pos + size, SEEK_SET);
            if (size & 1)
                bytestream2_skip(&ctx->gb, 1);
        }
        if (to_store)
            memcpy(ctx->stored_frame, ctx->frm0, ctx->buf_size);
        if ((ret = copy_output(ctx, NULL)))
            return ret;
        memcpy(ctx->output->data[1], ctx->pal, 1024);
    } else {
        SANMFrameHeader header;

        if ((ret = read_frame_header(ctx, &header)))
            return ret;

        ctx->rotate_code = header.rotate_code;
        if ((ctx->output->key_frame = !header.seq_num)) {
            ctx->output->pict_type = AV_PICTURE_TYPE_I;
            fill_frame(ctx->frm1, ctx->npixels, header.bg_color);
            fill_frame(ctx->frm2, ctx->npixels, header.bg_color);
        } else {
            ctx->output->pict_type = AV_PICTURE_TYPE_P;
        }

        if (header.codec < FF_ARRAY_ELEMS(v1_decoders)) {
            if ((ret = v1_decoders[header.codec](ctx))) {
                av_log(avctx, AV_LOG_ERROR,
                       "subcodec %d: error decoding frame\n", header.codec);
                return ret;
            }
        } else {
            av_log_ask_for_sample(avctx, "subcodec %d is not implemented\n",
                   header.codec);
            return AVERROR_PATCHWELCOME;
        }

        if ((ret = copy_output(ctx, &header)))
            return ret;
    }
    if (ctx->rotate_code)
        rotate_bufs(ctx, ctx->rotate_code);

    *got_frame_ptr  = 1;
    *(AVFrame*)data = *ctx->output;

    return pkt->size;
}
