static av_cold int decode_init(AVCodecContext *avctx)
{
    WMAProDecodeCtx *s = avctx->priv_data;
    uint8_t *edata_ptr = avctx->extradata;
    unsigned int channel_mask;
    int i, bits;
    int log2_max_num_subframes;
    int num_possible_block_sizes;

    if (avctx->codec_id == AV_CODEC_ID_XMA1 || avctx->codec_id == AV_CODEC_ID_XMA2)
        avctx->block_align = 2048;

    if (!avctx->block_align) {
        av_log(avctx, AV_LOG_ERROR, "block_align is not set\n");
        return AVERROR(EINVAL);
    }

    s->avctx = avctx;
    s->fdsp = avpriv_float_dsp_alloc(avctx->flags & AV_CODEC_FLAG_BITEXACT);
    if (!s->fdsp)
        return AVERROR(ENOMEM);

    init_put_bits(&s->pb, s->frame_data, MAX_FRAMESIZE);

    avctx->sample_fmt = AV_SAMPLE_FMT_FLTP;

    if (avctx->codec_id == AV_CODEC_ID_XMA2 && avctx->extradata_size >= 34) {
        s->decode_flags    = 0x10d6;
        channel_mask       = AV_RL32(edata_ptr+2);
        s->bits_per_sample = 16;
        /
        for (i = 0; i < avctx->extradata_size; i++)
            ff_dlog(avctx, "[%x] ", avctx->extradata[i]);
        ff_dlog(avctx, "\n");

     } else if (avctx->codec_id == AV_CODEC_ID_XMA1 && avctx->extradata_size >= 28) {
        s->decode_flags    = 0x10d6;
        s->bits_per_sample = 16;
        channel_mask       = 0;
        /
        for (i = 0; i < avctx->extradata_size; i++)
            ff_dlog(avctx, "[%x] ", avctx->extradata[i]);
        ff_dlog(avctx, "\n");

     } else if (avctx->extradata_size >= 18) {
        s->decode_flags    = AV_RL16(edata_ptr+14);
        channel_mask       = AV_RL32(edata_ptr+2);
        s->bits_per_sample = AV_RL16(edata_ptr);
        /
        for (i = 0; i < avctx->extradata_size; i++)
            ff_dlog(avctx, "[%x] ", avctx->extradata[i]);
        ff_dlog(avctx, "\n");

    } else {
        avpriv_request_sample(avctx, "Unknown extradata size");
        return AVERROR_PATCHWELCOME;
    }

    if (avctx->codec_id != AV_CODEC_ID_WMAPRO && avctx->channels > 2) {
        avpriv_report_missing_feature(avctx, ">2 channels support");
        return AVERROR_PATCHWELCOME;
    }

    /
    s->log2_frame_size = av_log2(avctx->block_align) + 4;
    if (s->log2_frame_size > 25) {
        avpriv_request_sample(avctx, "Large block align");
        return AVERROR_PATCHWELCOME;
    }

    /
    if (avctx->codec_id != AV_CODEC_ID_WMAPRO)
        s->skip_frame = 0;
    else
        s->skip_frame = 1; /

    s->packet_loss = 1;
    s->len_prefix  = (s->decode_flags & 0x40);

    /
    if (avctx->codec_id == AV_CODEC_ID_WMAPRO) {
        bits = ff_wma_get_frame_len_bits(avctx->sample_rate, 3, s->decode_flags);
        if (bits > WMAPRO_BLOCK_MAX_BITS) {
            avpriv_request_sample(avctx, "14-bit block sizes");
            return AVERROR_PATCHWELCOME;
        }
        s->samples_per_frame = 1 << bits;
    } else {
        s->samples_per_frame = 512;
    }

    /
    log2_max_num_subframes       = ((s->decode_flags & 0x38) >> 3);
    s->max_num_subframes         = 1 << log2_max_num_subframes;
    if (s->max_num_subframes == 16 || s->max_num_subframes == 4)
        s->max_subframe_len_bit = 1;
    s->subframe_len_bits = av_log2(log2_max_num_subframes) + 1;

    num_possible_block_sizes     = log2_max_num_subframes + 1;
    s->min_samples_per_subframe  = s->samples_per_frame / s->max_num_subframes;
    s->dynamic_range_compression = (s->decode_flags & 0x80);

    if (s->max_num_subframes > MAX_SUBFRAMES) {
        av_log(avctx, AV_LOG_ERROR, "invalid number of subframes %"PRId8"\n",
               s->max_num_subframes);
        return AVERROR_INVALIDDATA;
    }

    if (s->min_samples_per_subframe < WMAPRO_BLOCK_MIN_SIZE) {
        av_log(avctx, AV_LOG_ERROR, "min_samples_per_subframe of %d too small\n",
               s->min_samples_per_subframe);
        return AVERROR_INVALIDDATA;
    }

    if (s->avctx->sample_rate <= 0) {
        av_log(avctx, AV_LOG_ERROR, "invalid sample rate\n");
        return AVERROR_INVALIDDATA;
    }

    if (avctx->channels < 0) {
        av_log(avctx, AV_LOG_ERROR, "invalid number of channels %d\n",
               avctx->channels);
        return AVERROR_INVALIDDATA;
    } else if (avctx->channels > WMAPRO_MAX_CHANNELS) {
        avpriv_request_sample(avctx,
                              "More than %d channels", WMAPRO_MAX_CHANNELS);
        return AVERROR_PATCHWELCOME;
    }

    /
    for (i = 0; i < avctx->channels; i++)
        s->channel[i].prev_block_len = s->samples_per_frame;

    /
    s->lfe_channel = -1;

    if (channel_mask & 8) {
        unsigned int mask;
        for (mask = 1; mask < 16; mask <<= 1) {
            if (channel_mask & mask)
                ++s->lfe_channel;
        }
    }

    INIT_VLC_STATIC(&sf_vlc, SCALEVLCBITS, HUFF_SCALE_SIZE,
                    scale_huffbits, 1, 1,
                    scale_huffcodes, 2, 2, 616);

    INIT_VLC_STATIC(&sf_rl_vlc, VLCBITS, HUFF_SCALE_RL_SIZE,
                    scale_rl_huffbits, 1, 1,
                    scale_rl_huffcodes, 4, 4, 1406);

    INIT_VLC_STATIC(&coef_vlc[0], VLCBITS, HUFF_COEF0_SIZE,
                    coef0_huffbits, 1, 1,
                    coef0_huffcodes, 4, 4, 2108);

    INIT_VLC_STATIC(&coef_vlc[1], VLCBITS, HUFF_COEF1_SIZE,
                    coef1_huffbits, 1, 1,
                    coef1_huffcodes, 4, 4, 3912);

    INIT_VLC_STATIC(&vec4_vlc, VLCBITS, HUFF_VEC4_SIZE,
                    vec4_huffbits, 1, 1,
                    vec4_huffcodes, 2, 2, 604);

    INIT_VLC_STATIC(&vec2_vlc, VLCBITS, HUFF_VEC2_SIZE,
                    vec2_huffbits, 1, 1,
                    vec2_huffcodes, 2, 2, 562);

    INIT_VLC_STATIC(&vec1_vlc, VLCBITS, HUFF_VEC1_SIZE,
                    vec1_huffbits, 1, 1,
                    vec1_huffcodes, 2, 2, 562);

    /
    for (i = 0; i < num_possible_block_sizes; i++) {
        int subframe_len = s->samples_per_frame >> i;
        int x;
        int band = 1;
        int rate = get_rate(avctx);

        s->sfb_offsets[i][0] = 0;

        for (x = 0; x < MAX_BANDS-1 && s->sfb_offsets[i][band - 1] < subframe_len; x++) {
            int offset = (subframe_len * 2 * critical_freq[x]) / rate + 2;
            offset &= ~3;
            if (offset > s->sfb_offsets[i][band - 1])
                s->sfb_offsets[i][band++] = offset;

            if (offset >= subframe_len)
                break;
        }
        s->sfb_offsets[i][band - 1] = subframe_len;
        s->num_sfb[i]               = band - 1;
        if (s->num_sfb[i] <= 0) {
            av_log(avctx, AV_LOG_ERROR, "num_sfb invalid\n");
            return AVERROR_INVALIDDATA;
        }
    }


    /

    for (i = 0; i < num_possible_block_sizes; i++) {
        int b;
        for (b = 0; b < s->num_sfb[i]; b++) {
            int x;
            int offset = ((s->sfb_offsets[i][b]
                           + s->sfb_offsets[i][b + 1] - 1) << i) >> 1;
            for (x = 0; x < num_possible_block_sizes; x++) {
                int v = 0;
                while (s->sfb_offsets[x][v + 1] << x < offset) {
                    v++;
                    av_assert0(v < MAX_BANDS);
                }
                s->sf_offsets[i][x][b] = v;
            }
        }
    }

    /
    for (i = 0; i < WMAPRO_BLOCK_SIZES; i++)
        ff_mdct_init(&s->mdct_ctx[i], WMAPRO_BLOCK_MIN_BITS+1+i, 1,
                     1.0 / (1 << (WMAPRO_BLOCK_MIN_BITS + i - 1))
                     / (1 << (s->bits_per_sample - 1)));

    /
    for (i = 0; i < WMAPRO_BLOCK_SIZES; i++) {
        const int win_idx = WMAPRO_BLOCK_MAX_BITS - i;
        ff_init_ff_sine_windows(win_idx);
        s->windows[WMAPRO_BLOCK_SIZES - i - 1] = ff_sine_windows[win_idx];
    }

    /
    for (i = 0; i < num_possible_block_sizes; i++) {
        int block_size = s->samples_per_frame >> i;
        int cutoff = (440*block_size + 3 * (s->avctx->sample_rate >> 1) - 1)
                     / s->avctx->sample_rate;
        s->subwoofer_cutoffs[i] = av_clip(cutoff, 4, block_size);
    }

    /
    for (i = 0; i < 33; i++)
        sin64[i] = sin(i*M_PI / 64.0);

    if (avctx->debug & FF_DEBUG_BITSTREAM)
        dump_context(s);

    avctx->channel_layout = channel_mask;

    return 0;
}
