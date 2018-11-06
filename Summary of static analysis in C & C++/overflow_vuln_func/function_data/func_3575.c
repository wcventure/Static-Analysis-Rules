static int opus_decode_packet(AVCodecContext *avctx, void *data,
                              int *got_frame_ptr, AVPacket *avpkt)
{
    OpusContext *c      = avctx->priv_data;
    AVFrame *frame      = data;
    const uint8_t *buf  = avpkt->data;
    int buf_size        = avpkt->size;
    int coded_samples   = 0;
    int decoded_samples = 0;
    int i, ret;

    for (i = 0; i < c->nb_streams; i++) {
        OpusStreamContext *s = &c->streams[i];
        s->out[0] =
        s->out[1] = NULL;
    }

    /
    if (buf) {
        OpusPacket *pkt = &c->streams[0].packet;
        ret = ff_opus_parse_packet(pkt, buf, buf_size, c->nb_streams > 1);
        if (ret < 0) {
            av_log(avctx, AV_LOG_ERROR, "Error parsing the packet header.\n");
            return ret;
        }
        coded_samples += pkt->frame_count * pkt->frame_duration;
        c->streams[0].silk_samplerate = get_silk_samplerate(pkt->config);
    }

    frame->nb_samples = coded_samples + c->streams[0].delayed_samples;

    /
    if (!frame->nb_samples) {
        *got_frame_ptr = 0;
        return 0;
    }

    /
    ret = ff_get_buffer(avctx, frame, 0);
    if (ret < 0)
        return ret;
    frame->nb_samples = 0;

    for (i = 0; i < avctx->channels; i++) {
        ChannelMap *map = &c->channel_maps[i];
        if (!map->copy)
            c->streams[map->stream_idx].out[map->channel_idx] = (float*)frame->extended_data[i];
    }

    for (i = 0; i < c->nb_streams; i++)
        c->streams[i].out_size = frame->linesize[0];

    /
    for (i = 0; i < c->nb_streams; i++) {
        OpusStreamContext *s = &c->streams[i];

        if (i && buf) {
            ret = ff_opus_parse_packet(&s->packet, buf, buf_size, i != c->nb_streams - 1);
            if (ret < 0) {
                av_log(avctx, AV_LOG_ERROR, "Error parsing the packet header.\n");
                return ret;
            }
            if (coded_samples != s->packet.frame_count * s->packet.frame_duration) {
                av_log(avctx, AV_LOG_ERROR,
                       "Mismatching coded sample count in substream %d.\n", i);
                return AVERROR_INVALIDDATA;
            }

            s->silk_samplerate = get_silk_samplerate(s->packet.config);
        }

        ret = opus_decode_subpacket(&c->streams[i], buf,
                                    s->packet.data_size, coded_samples);
        if (ret < 0)
            return ret;
        if (decoded_samples && ret != decoded_samples) {
            av_log(avctx, AV_LOG_ERROR, "Different numbers of decoded samples "
                   "in a multi-channel stream\n");
            return AVERROR_INVALIDDATA;
        }
        decoded_samples = ret;
        buf      += s->packet.packet_size;
        buf_size -= s->packet.packet_size;
    }

    for (i = 0; i < avctx->channels; i++) {
        ChannelMap *map = &c->channel_maps[i];

        /
        if (map->copy) {
            memcpy(frame->extended_data[i],
                   frame->extended_data[map->copy_idx],
                   frame->linesize[0]);
        } else if (map->silence) {
            memset(frame->extended_data[i], 0, frame->linesize[0]);
        }

        if (c->gain_i) {
            c->fdsp->vector_fmul_scalar((float*)frame->extended_data[i],
                                       (float*)frame->extended_data[i],
                                       c->gain, FFALIGN(decoded_samples, 8));
        }
    }

    frame->nb_samples = decoded_samples;
    *got_frame_ptr    = !!decoded_samples;

    return avpkt->size;
}
