static int applehttp_read_header(AVFormatContext *s, AVFormatParameters *ap)
{
    AppleHTTPContext *c = s->priv_data;
    int ret = 0, i, j, stream_offset = 0;

    if ((ret = parse_playlist(c, s->filename, NULL, s->pb)) < 0)
        goto fail;

    if (c->n_variants == 0) {
        av_log(NULL, AV_LOG_WARNING, "Empty playlist\n");
        ret = AVERROR_EOF;
        goto fail;
    }
    /
    if (c->n_variants > 1 || c->variants[0]->n_segments == 0) {
        for (i = 0; i < c->n_variants; i++) {
            struct variant *v = c->variants[i];
            if ((ret = parse_playlist(c, v->url, v, NULL)) < 0)
                goto fail;
        }
    }

    if (c->variants[0]->n_segments == 0) {
        av_log(NULL, AV_LOG_WARNING, "Empty playlist\n");
        ret = AVERROR_EOF;
        goto fail;
    }

    /
    if (c->finished) {
        int duration = 0;
        for (i = 0; i < c->variants[0]->n_segments; i++)
            duration += c->variants[0]->segments[i]->duration;
        s->duration = duration * AV_TIME_BASE;
    }

    c->min_end_seq = INT_MAX;
    /
    for (i = 0; i < c->n_variants; i++) {
        struct variant *v = c->variants[i];
        if (v->n_segments == 0)
            continue;
        c->max_start_seq = FFMAX(c->max_start_seq, v->start_seq_no);
        c->min_end_seq   = FFMIN(c->min_end_seq,   v->start_seq_no +
                                                   v->n_segments);
        ret = av_open_input_file(&v->ctx, v->segments[0]->url, NULL, 0, NULL);
        if (ret < 0)
            goto fail;
        url_fclose(v->ctx->pb);
        v->ctx->pb = NULL;
        v->stream_offset = stream_offset;
        /
        for (j = 0; j < v->ctx->nb_streams; j++) {
            AVStream *st = av_new_stream(s, i);
            if (!st) {
                ret = AVERROR(ENOMEM);
                goto fail;
            }
            avcodec_copy_context(st->codec, v->ctx->streams[j]->codec);
        }
        stream_offset += v->ctx->nb_streams;
    }
    c->last_packet_dts = AV_NOPTS_VALUE;

    c->cur_seq_no = c->max_start_seq;
    /
    if (!c->finished && c->min_end_seq - c->max_start_seq > 3)
        c->cur_seq_no = c->min_end_seq - 2;

    return 0;
fail:
    free_variant_list(c);
    return ret;
}
