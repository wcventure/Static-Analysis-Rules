static int vp9_superframe_split_filter(AVBSFContext *ctx, AVPacket *out)
{
    VP9SFSplitContext *s = ctx->priv_data;
    AVPacket *in;
    int i, j, ret, marker;
    int is_superframe = !!s->buffer_pkt;

    if (!s->buffer_pkt) {
        ret = ff_bsf_get_packet(ctx, &s->buffer_pkt);
        if (ret < 0)
            return ret;
        in = s->buffer_pkt;

        marker = in->data[in->size - 1];
        if ((marker & 0xe0) == 0xc0) {
            int length_size = 1 + ((marker >> 3) & 0x3);
            int   nb_frames = 1 + (marker & 0x7);
            int    idx_size = 2 + nb_frames * length_size;

            if (in->size >= idx_size && in->data[in->size - idx_size] == marker) {
                GetByteContext bc;
                int total_size = 0;

                bytestream2_init(&bc, in->data + in->size + 1 - idx_size,
                                 nb_frames * length_size);

                for (i = 0; i < nb_frames; i++) {
                    int frame_size = 0;
                    for (j = 0; j < length_size; j++)
                        frame_size |= bytestream2_get_byte(&bc) << (j * 8);

                    total_size += frame_size;
                    if (total_size > in->size - idx_size) {
                        av_log(ctx, AV_LOG_ERROR,
                               "Invalid frame size in a superframe: %d\n", frame_size);
                        ret = AVERROR(EINVAL);
                        goto fail;
                    }
                    s->sizes[i] = frame_size;
                }
                s->nb_frames         = nb_frames;
                s->next_frame        = 0;
                s->next_frame_offset = 0;
                is_superframe        = 1;
            }
        }
    }

    if (is_superframe) {
        GetBitContext gb;
        int profile, invisible = 0;

        ret = av_packet_ref(out, s->buffer_pkt);
        if (ret < 0)
            goto fail;

        out->data += s->next_frame_offset;
        out->size  = s->sizes[s->next_frame];

        s->next_frame_offset += out->size;
        s->next_frame++;

        if (s->next_frame >= s->nb_frames)
            av_packet_free(&s->buffer_pkt);

        ret = init_get_bits8(&gb, out->data, out->size);
        if (ret < 0)
            goto fail;

        get_bits(&gb, 2); // frame_marker
        profile  = get_bits1(&gb);
        profile |= get_bits1(&gb) << 1;
        if (profile == 3)
            get_bits1(&gb);
        if (!get_bits1(&gb)) {
            get_bits1(&gb);
            invisible = !get_bits1(&gb);
        }

        if (invisible)
            out->pts = AV_NOPTS_VALUE;

    } else {
        av_packet_move_ref(out, s->buffer_pkt);
        av_packet_free(&s->buffer_pkt);
    }

    return 0;
fail:
    av_packet_free(&s->buffer_pkt);
    return ret;
}
