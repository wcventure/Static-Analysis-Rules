static int wmavoice_decode_packet(AVCodecContext *ctx, void *data,
                                  int *got_frame_ptr, AVPacket *avpkt)
{
    WMAVoiceContext *s = ctx->priv_data;
    GetBitContext *gb = &s->gb;
    int size, res, pos;

    /
    for (size = avpkt->size; size > ctx->block_align; size -= ctx->block_align);
    init_get_bits(&s->gb, avpkt->data, size << 3);

    /
    if (!(size % ctx->block_align)) { // new packet header
        if (!size) {
            s->spillover_nbits = 0;
            s->nb_superframes = 0;
        } else {
            if ((res = parse_packet_header(s)) < 0)
                return res;
            s->nb_superframes = res;
        }

        /
        if (s->sframe_cache_size > 0) {
            int cnt = get_bits_count(gb);
            copy_bits(&s->pb, avpkt->data, size, gb, s->spillover_nbits);
            flush_put_bits(&s->pb);
            s->sframe_cache_size += s->spillover_nbits;
            if ((res = synth_superframe(ctx, data, got_frame_ptr)) == 0 &&
                *got_frame_ptr) {
                cnt += s->spillover_nbits;
                s->skip_bits_next = cnt & 7;
                res = cnt >> 3;
                if (res > avpkt->size) {
                    av_log(ctx, AV_LOG_ERROR,
                           "Trying to skip %d bytes in packet of size %d\n",
                           res, avpkt->size);
                    return AVERROR_INVALIDDATA;
                }
                return res;
            } else
                skip_bits_long (gb, s->spillover_nbits - cnt +
                                get_bits_count(gb)); // resync
        } else if (s->spillover_nbits) {
            skip_bits_long(gb, s->spillover_nbits);  // resync
        }
    } else if (s->skip_bits_next)
        skip_bits(gb, s->skip_bits_next);

    /
    s->sframe_cache_size = 0;
    s->skip_bits_next = 0;
    pos = get_bits_left(gb);
    if (s->nb_superframes-- == 0) {
        *got_frame_ptr = 0;
        return size;
    } else if (s->nb_superframes > 0) {
        if ((res = synth_superframe(ctx, data, got_frame_ptr)) < 0) {
            return res;
        } else if (*got_frame_ptr) {
            int cnt = get_bits_count(gb);
            s->skip_bits_next = cnt & 7;
            res = cnt >> 3;
            if (res > avpkt->size) {
                av_log(ctx, AV_LOG_ERROR,
                       "Trying to skip %d bytes in packet of size %d\n",
                       res, avpkt->size);
                return AVERROR_INVALIDDATA;
            }
            return res;
        }
    } else if ((s->sframe_cache_size = pos) > 0) {
        /
        init_put_bits(&s->pb, s->sframe_cache, SFRAME_CACHE_MAXSIZE);
        copy_bits(&s->pb, avpkt->data, size, gb, s->sframe_cache_size);
        // FIXME bad - just copy bytes as whole and add use the
        // skip_bits_next field
    }

    return size;
}
