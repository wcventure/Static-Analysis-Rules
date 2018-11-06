static int tm2_read_stream(TM2Context *ctx, const uint8_t *buf, int stream_id, int buf_size)
{
    int i, ret;
    int skip = 0;
    int len, toks, pos;
    TM2Codes codes;
    GetByteContext gb;

    if (buf_size < 4) {
        av_log(ctx->avctx, AV_LOG_ERROR, "not enough space for len left\n");
        return AVERROR_INVALIDDATA;
    }

    /
    bytestream2_init(&gb, buf, buf_size);
    len  = bytestream2_get_be32(&gb);
    skip = len * 4 + 4;

    if (len == 0)
        return 4;

    if (len >= INT_MAX / 4 - 1 || len < 0 || skip > buf_size) {
        av_log(ctx->avctx, AV_LOG_ERROR, "Error, invalid stream size.\n");
        return AVERROR_INVALIDDATA;
    }

    toks = bytestream2_get_be32(&gb);
    if (toks & 1) {
        len = bytestream2_get_be32(&gb);
        if (len == TM2_ESCAPE) {
            len = bytestream2_get_be32(&gb);
        }
        if (len > 0) {
            pos = bytestream2_tell(&gb);
            if (skip <= pos)
                return AVERROR_INVALIDDATA;
            init_get_bits(&ctx->gb, buf + pos, (skip - pos) * 8);
            if ((ret = tm2_read_deltas(ctx, stream_id)) < 0)
                return ret;
            bytestream2_skip(&gb, ((get_bits_count(&ctx->gb) + 31) >> 5) << 2);
        }
    }
    /
    len = bytestream2_get_be32(&gb);
    if (len == TM2_ESCAPE) { /
        bytestream2_skip(&gb, 8); /
    } else {
        bytestream2_skip(&gb, 4); /
    }

    pos = bytestream2_tell(&gb);
    if (skip <= pos)
        return AVERROR_INVALIDDATA;
    init_get_bits(&ctx->gb, buf + pos, (skip - pos) * 8);
    if ((ret = tm2_build_huff_table(ctx, &codes)) < 0)
        return ret;
    bytestream2_skip(&gb, ((get_bits_count(&ctx->gb) + 31) >> 5) << 2);

    toks >>= 1;
    /
    if ((toks < 0) || (toks > 0xFFFFFF)) {
        av_log(ctx->avctx, AV_LOG_ERROR, "Incorrect number of tokens: %i\n", toks);
        ret = AVERROR_INVALIDDATA;
        goto end;
    }
    ret = av_reallocp_array(&ctx->tokens[stream_id], toks, sizeof(int));
    if (ret < 0) {
        ctx->tok_lens[stream_id] = 0;
        goto end;
    }
    ctx->tok_lens[stream_id] = toks;
    len = bytestream2_get_be32(&gb);
    if (len > 0) {
        pos = bytestream2_tell(&gb);
        if (skip <= pos) {
            ret = AVERROR_INVALIDDATA;
            goto end;
        }
        init_get_bits(&ctx->gb, buf + pos, (skip - pos) * 8);
        for (i = 0; i < toks; i++) {
            if (get_bits_left(&ctx->gb) <= 0) {
                av_log(ctx->avctx, AV_LOG_ERROR, "Incorrect number of tokens: %i\n", toks);
                ret = AVERROR_INVALIDDATA;
                goto end;
            }
            ctx->tokens[stream_id][i] = tm2_get_token(&ctx->gb, &codes);
            if (stream_id <= TM2_MOT && ctx->tokens[stream_id][i] >= TM2_DELTAS || ctx->tokens[stream_id][i]<0) {
                av_log(ctx->avctx, AV_LOG_ERROR, "Invalid delta token index %d for type %d, n=%d\n",
                       ctx->tokens[stream_id][i], stream_id, i);
                ret = AVERROR_INVALIDDATA;
                goto end;
            }
        }
    } else {
        for (i = 0; i < toks; i++) {
            ctx->tokens[stream_id][i] = codes.recode[0];
            if (stream_id <= TM2_MOT && ctx->tokens[stream_id][i] >= TM2_DELTAS) {
                av_log(ctx->avctx, AV_LOG_ERROR, "Invalid delta token index %d for type %d, n=%d\n",
                       ctx->tokens[stream_id][i], stream_id, i);
                ret = AVERROR_INVALIDDATA;
                goto end;
            }
        }
    }

    ret = skip;

end:
    tm2_free_codes(&codes);
    return ret;
}
