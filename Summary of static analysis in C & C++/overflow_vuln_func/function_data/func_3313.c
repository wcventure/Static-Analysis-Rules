static int adpcm_encode_frame(AVCodecContext *avctx, AVPacket *avpkt,
                              const AVFrame *frame, int *got_packet_ptr)
{
    int n, i, st, pkt_size, ret;
    const int16_t *samples;
    uint8_t *dst;
    ADPCMEncodeContext *c = avctx->priv_data;
    uint8_t *buf;

    samples = (const int16_t *)frame->data[0];
    st = avctx->channels == 2;

    if (avctx->codec_id == AV_CODEC_ID_ADPCM_SWF)
        pkt_size = (2 + avctx->channels * (22 + 4 * (frame->nb_samples - 1)) + 7) / 8;
    else
        pkt_size = avctx->block_align;
    if ((ret = ff_alloc_packet(avpkt, pkt_size))) {
        av_log(avctx, AV_LOG_ERROR, "Error getting output packet\n");
        return ret;
    }
    dst = avpkt->data;

    switch(avctx->codec->id) {
    case AV_CODEC_ID_ADPCM_IMA_WAV:
        n = frame->nb_samples / 8;
        c->status[0].prev_sample = samples[0];
        /
        bytestream_put_le16(&dst, c->status[0].prev_sample);
        *dst++ = c->status[0].step_index;
        *dst++ = 0; /
        samples++;
        if (avctx->channels == 2) {
            c->status[1].prev_sample = samples[0];
            /
            bytestream_put_le16(&dst, c->status[1].prev_sample);
            *dst++ = c->status[1].step_index;
            *dst++ = 0;
            samples++;
        }

        /
        if (avctx->trellis > 0) {
            FF_ALLOC_OR_GOTO(avctx, buf, 2 * n * 8, error);
            adpcm_compress_trellis(avctx, samples, buf, &c->status[0], n * 8);
            if (avctx->channels == 2)
                adpcm_compress_trellis(avctx, samples + 1, buf + n * 8,
                                       &c->status[1], n * 8);
            for (i = 0; i < n; i++) {
                *dst++ = buf[8 * i + 0] | (buf[8 * i + 1] << 4);
                *dst++ = buf[8 * i + 2] | (buf[8 * i + 3] << 4);
                *dst++ = buf[8 * i + 4] | (buf[8 * i + 5] << 4);
                *dst++ = buf[8 * i + 6] | (buf[8 * i + 7] << 4);
                if (avctx->channels == 2) {
                    uint8_t *buf1 = buf + n * 8;
                    *dst++ = buf1[8 * i + 0] | (buf1[8 * i + 1] << 4);
                    *dst++ = buf1[8 * i + 2] | (buf1[8 * i + 3] << 4);
                    *dst++ = buf1[8 * i + 4] | (buf1[8 * i + 5] << 4);
                    *dst++ = buf1[8 * i + 6] | (buf1[8 * i + 7] << 4);
                }
            }
            av_free(buf);
        } else {
            for (; n > 0; n--) {
                *dst    = adpcm_ima_compress_sample(&c->status[0], samples[0]);
                *dst++ |= adpcm_ima_compress_sample(&c->status[0], samples[avctx->channels    ]) << 4;
                *dst    = adpcm_ima_compress_sample(&c->status[0], samples[avctx->channels * 2]);
                *dst++ |= adpcm_ima_compress_sample(&c->status[0], samples[avctx->channels * 3]) << 4;
                *dst    = adpcm_ima_compress_sample(&c->status[0], samples[avctx->channels * 4]);
                *dst++ |= adpcm_ima_compress_sample(&c->status[0], samples[avctx->channels * 5]) << 4;
                *dst    = adpcm_ima_compress_sample(&c->status[0], samples[avctx->channels * 6]);
                *dst++ |= adpcm_ima_compress_sample(&c->status[0], samples[avctx->channels * 7]) << 4;
                /
                if (avctx->channels == 2) {
                    *dst    = adpcm_ima_compress_sample(&c->status[1], samples[1 ]);
                    *dst++ |= adpcm_ima_compress_sample(&c->status[1], samples[3 ]) << 4;
                    *dst    = adpcm_ima_compress_sample(&c->status[1], samples[5 ]);
                    *dst++ |= adpcm_ima_compress_sample(&c->status[1], samples[7 ]) << 4;
                    *dst    = adpcm_ima_compress_sample(&c->status[1], samples[9 ]);
                    *dst++ |= adpcm_ima_compress_sample(&c->status[1], samples[11]) << 4;
                    *dst    = adpcm_ima_compress_sample(&c->status[1], samples[13]);
                    *dst++ |= adpcm_ima_compress_sample(&c->status[1], samples[15]) << 4;
                }
                samples += 8 * avctx->channels;
            }
        }
        break;
    case AV_CODEC_ID_ADPCM_IMA_QT:
    {
        int ch, i;
        PutBitContext pb;
        init_put_bits(&pb, dst, pkt_size * 8);

        for (ch = 0; ch < avctx->channels; ch++) {
            put_bits(&pb, 9, (c->status[ch].prev_sample + 0x10000) >> 7);
            put_bits(&pb, 7,  c->status[ch].step_index);
            if (avctx->trellis > 0) {
                uint8_t buf[64];
                adpcm_compress_trellis(avctx, samples+ch, buf, &c->status[ch], 64);
                for (i = 0; i < 64; i++)
                    put_bits(&pb, 4, buf[i ^ 1]);
            } else {
                for (i = 0; i < 64; i += 2) {
                    int t1, t2;
                    t1 = adpcm_ima_qt_compress_sample(&c->status[ch],
                                                      samples[avctx->channels * (i + 0) + ch]);
                    t2 = adpcm_ima_qt_compress_sample(&c->status[ch],
                                                      samples[avctx->channels * (i + 1) + ch]);
                    put_bits(&pb, 4, t2);
                    put_bits(&pb, 4, t1);
                }
            }
        }

        flush_put_bits(&pb);
        break;
    }
    case AV_CODEC_ID_ADPCM_SWF:
    {
        int i;
        PutBitContext pb;
        init_put_bits(&pb, dst, pkt_size * 8);

        n = frame->nb_samples - 1;

        // store AdpcmCodeSize
        put_bits(&pb, 2, 2);    // set 4-bit flash adpcm format

        // init the encoder state
        for (i = 0; i < avctx->channels; i++) {
            // clip step so it fits 6 bits
            c->status[i].step_index = av_clip(c->status[i].step_index, 0, 63);
            put_sbits(&pb, 16, samples[i]);
            put_bits(&pb, 6, c->status[i].step_index);
            c->status[i].prev_sample = samples[i];
        }

        if (avctx->trellis > 0) {
            FF_ALLOC_OR_GOTO(avctx, buf, 2 * n, error);
            adpcm_compress_trellis(avctx, samples + avctx->channels, buf,
                                   &c->status[0], n);
            if (avctx->channels == 2)
                adpcm_compress_trellis(avctx, samples + avctx->channels + 1,
                                       buf + n, &c->status[1], n);
            for (i = 0; i < n; i++) {
                put_bits(&pb, 4, buf[i]);
                if (avctx->channels == 2)
                    put_bits(&pb, 4, buf[n + i]);
            }
            av_free(buf);
        } else {
            for (i = 1; i < frame->nb_samples; i++) {
                put_bits(&pb, 4, adpcm_ima_compress_sample(&c->status[0],
                         samples[avctx->channels * i]));
                if (avctx->channels == 2)
                    put_bits(&pb, 4, adpcm_ima_compress_sample(&c->status[1],
                             samples[2 * i + 1]));
            }
        }
        flush_put_bits(&pb);
        break;
    }
    case AV_CODEC_ID_ADPCM_MS:
        for (i = 0; i < avctx->channels; i++) {
            int predictor = 0;
            *dst++ = predictor;
            c->status[i].coeff1 = ff_adpcm_AdaptCoeff1[predictor];
            c->status[i].coeff2 = ff_adpcm_AdaptCoeff2[predictor];
        }
        for (i = 0; i < avctx->channels; i++) {
            if (c->status[i].idelta < 16)
                c->status[i].idelta = 16;
            bytestream_put_le16(&dst, c->status[i].idelta);
        }
        for (i = 0; i < avctx->channels; i++)
            c->status[i].sample2= *samples++;
        for (i = 0; i < avctx->channels; i++) {
            c->status[i].sample1 = *samples++;
            bytestream_put_le16(&dst, c->status[i].sample1);
        }
        for (i = 0; i < avctx->channels; i++)
            bytestream_put_le16(&dst, c->status[i].sample2);

        if (avctx->trellis > 0) {
            int n = avctx->block_align - 7 * avctx->channels;
            FF_ALLOC_OR_GOTO(avctx, buf, 2 * n, error);
            if (avctx->channels == 1) {
                adpcm_compress_trellis(avctx, samples, buf, &c->status[0], n);
                for (i = 0; i < n; i += 2)
                    *dst++ = (buf[i] << 4) | buf[i + 1];
            } else {
                adpcm_compress_trellis(avctx, samples,     buf,     &c->status[0], n);
                adpcm_compress_trellis(avctx, samples + 1, buf + n, &c->status[1], n);
                for (i = 0; i < n; i++)
                    *dst++ = (buf[i] << 4) | buf[n + i];
            }
            av_free(buf);
        } else {
            for (i = 7 * avctx->channels; i < avctx->block_align; i++) {
                int nibble;
                nibble  = adpcm_ms_compress_sample(&c->status[ 0], *samples++) << 4;
                nibble |= adpcm_ms_compress_sample(&c->status[st], *samples++);
                *dst++  = nibble;
            }
        }
        break;
    case AV_CODEC_ID_ADPCM_YAMAHA:
        n = frame->nb_samples / 2;
        if (avctx->trellis > 0) {
            FF_ALLOC_OR_GOTO(avctx, buf, 2 * n * 2, error);
            n *= 2;
            if (avctx->channels == 1) {
                adpcm_compress_trellis(avctx, samples, buf, &c->status[0], n);
                for (i = 0; i < n; i += 2)
                    *dst++ = buf[i] | (buf[i + 1] << 4);
            } else {
                adpcm_compress_trellis(avctx, samples,     buf,     &c->status[0], n);
                adpcm_compress_trellis(avctx, samples + 1, buf + n, &c->status[1], n);
                for (i = 0; i < n; i++)
                    *dst++ = buf[i] | (buf[n + i] << 4);
            }
            av_free(buf);
        } else
            for (n *= avctx->channels; n > 0; n--) {
                int nibble;
                nibble  = adpcm_yamaha_compress_sample(&c->status[ 0], *samples++);
                nibble |= adpcm_yamaha_compress_sample(&c->status[st], *samples++) << 4;
                *dst++  = nibble;
            }
        break;
    default:
        return AVERROR(EINVAL);
    }

    avpkt->size = pkt_size;
    *got_packet_ptr = 1;
    return 0;
error:
    return AVERROR(ENOMEM);
}
