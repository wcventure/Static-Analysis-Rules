static int wavpack_decode_block(AVCodecContext *avctx, int block_no,
                                void *data, int *got_frame_ptr,
                                const uint8_t *buf, int buf_size)
{
    WavpackContext *wc = avctx->priv_data;
    WavpackFrameContext *s;
    void *samples = data;
    int samplecount;
    int got_terms = 0, got_weights = 0, got_samples = 0, got_entropy = 0, got_bs = 0, got_float = 0;
    int got_hybrid = 0;
    const uint8_t* orig_buf = buf;
    const uint8_t* buf_end = buf + buf_size;
    int i, j, id, size, ssize, weights, t;
    int bpp, chan, chmask;

    if (buf_size == 0){
        *got_frame_ptr = 0;
        return 0;
    }

    if(block_no >= wc->fdec_num && wv_alloc_frame_context(wc) < 0){
        av_log(avctx, AV_LOG_ERROR, "Error creating frame decode context\n");
        return -1;
    }

    s = wc->fdec[block_no];
    if(!s){
        av_log(avctx, AV_LOG_ERROR, "Context for block %d is not present\n", block_no);
        return -1;
    }

        memset(s->decorr, 0, MAX_TERMS * sizeof(Decorr));
        memset(s->ch, 0, sizeof(s->ch));
        s->extra_bits = 0;
        s->and = s->or = s->shift = 0;
        s->got_extra_bits = 0;

    if(!wc->mkv_mode){
        s->samples = AV_RL32(buf); buf += 4;
        if(!s->samples){
            *got_frame_ptr = 0;
            return 0;
        }
    }else{
        s->samples = wc->samples;
    }
    s->frame_flags = AV_RL32(buf); buf += 4;
    if(s->frame_flags&0x80){
        avctx->sample_fmt = AV_SAMPLE_FMT_FLT;
    } else if((s->frame_flags&0x03) <= 1){
        avctx->sample_fmt = AV_SAMPLE_FMT_S16;
    } else {
        avctx->sample_fmt = AV_SAMPLE_FMT_S32;
    }
    bpp = av_get_bytes_per_sample(avctx->sample_fmt);
    samples = (uint8_t*)samples + bpp * wc->ch_offset;

    s->stereo = !(s->frame_flags & WV_MONO);
    s->stereo_in = (s->frame_flags & WV_FALSE_STEREO) ? 0 : s->stereo;
    s->joint = s->frame_flags & WV_JOINT_STEREO;
    s->hybrid = s->frame_flags & WV_HYBRID_MODE;
    s->hybrid_bitrate = s->frame_flags & WV_HYBRID_BITRATE;
    s->hybrid_maxclip = 1 << ((((s->frame_flags & 0x03) + 1) << 3) - 1);
    s->post_shift = 8 * (bpp-1-(s->frame_flags&0x03)) + ((s->frame_flags >> 13) & 0x1f);
    s->CRC = AV_RL32(buf); buf += 4;
    if(wc->mkv_mode)
        buf += 4; //skip block size;

    wc->ch_offset += 1 + s->stereo;

    // parse metadata blocks
    while(buf < buf_end){
        id = *buf++;
        size = *buf++;
        if(id & WP_IDF_LONG) {
            size |= (*buf++) << 8;
            size |= (*buf++) << 16;
        }
        size <<= 1; // size is specified in words
        ssize = size;
        if(id & WP_IDF_ODD) size--;
        if(size < 0){
            av_log(avctx, AV_LOG_ERROR, "Got incorrect block %02X with size %i\n", id, size);
            break;
        }
        if(buf + ssize > buf_end){
            av_log(avctx, AV_LOG_ERROR, "Block size %i is out of bounds\n", size);
            break;
        }
        if(id & WP_IDF_IGNORE){
            buf += ssize;
            continue;
        }
        switch(id & WP_IDF_MASK){
        case WP_ID_DECTERMS:
            if(size > MAX_TERMS){
                av_log(avctx, AV_LOG_ERROR, "Too many decorrelation terms\n");
                s->terms = 0;
                buf += ssize;
                continue;
            }
            s->terms = size;
            for(i = 0; i < s->terms; i++) {
                s->decorr[s->terms - i - 1].value = (*buf & 0x1F) - 5;
                s->decorr[s->terms - i - 1].delta = *buf >> 5;
                buf++;
            }
            got_terms = 1;
            break;
        case WP_ID_DECWEIGHTS:
            if(!got_terms){
                av_log(avctx, AV_LOG_ERROR, "No decorrelation terms met\n");
                continue;
            }
            weights = size >> s->stereo_in;
            if(weights > MAX_TERMS || weights > s->terms){
                av_log(avctx, AV_LOG_ERROR, "Too many decorrelation weights\n");
                buf += ssize;
                continue;
            }
            for(i = 0; i < weights; i++) {
                t = (int8_t)(*buf++);
                s->decorr[s->terms - i - 1].weightA = t << 3;
                if(s->decorr[s->terms - i - 1].weightA > 0)
                    s->decorr[s->terms - i - 1].weightA += (s->decorr[s->terms - i - 1].weightA + 64) >> 7;
                if(s->stereo_in){
                    t = (int8_t)(*buf++);
                    s->decorr[s->terms - i - 1].weightB = t << 3;
                    if(s->decorr[s->terms - i - 1].weightB > 0)
                        s->decorr[s->terms - i - 1].weightB += (s->decorr[s->terms - i - 1].weightB + 64) >> 7;
                }
            }
            got_weights = 1;
            break;
        case WP_ID_DECSAMPLES:
            if(!got_terms){
                av_log(avctx, AV_LOG_ERROR, "No decorrelation terms met\n");
                continue;
            }
            t = 0;
            for(i = s->terms - 1; (i >= 0) && (t < size); i--) {
                if(s->decorr[i].value > 8){
                    s->decorr[i].samplesA[0] = wp_exp2(AV_RL16(buf)); buf += 2;
                    s->decorr[i].samplesA[1] = wp_exp2(AV_RL16(buf)); buf += 2;
                    if(s->stereo_in){
                        s->decorr[i].samplesB[0] = wp_exp2(AV_RL16(buf)); buf += 2;
                        s->decorr[i].samplesB[1] = wp_exp2(AV_RL16(buf)); buf += 2;
                        t += 4;
                    }
                    t += 4;
                }else if(s->decorr[i].value < 0){
                    s->decorr[i].samplesA[0] = wp_exp2(AV_RL16(buf)); buf += 2;
                    s->decorr[i].samplesB[0] = wp_exp2(AV_RL16(buf)); buf += 2;
                    t += 4;
                }else{
                    for(j = 0; j < s->decorr[i].value; j++){
                        s->decorr[i].samplesA[j] = wp_exp2(AV_RL16(buf)); buf += 2;
                        if(s->stereo_in){
                            s->decorr[i].samplesB[j] = wp_exp2(AV_RL16(buf)); buf += 2;
                        }
                    }
                    t += s->decorr[i].value * 2 * (s->stereo_in + 1);
                }
            }
            got_samples = 1;
            break;
        case WP_ID_ENTROPY:
            if(size != 6 * (s->stereo_in + 1)){
                av_log(avctx, AV_LOG_ERROR, "Entropy vars size should be %i, got %i", 6 * (s->stereo_in + 1), size);
                buf += ssize;
                continue;
            }
            for(j = 0; j <= s->stereo_in; j++){
                for(i = 0; i < 3; i++){
                    s->ch[j].median[i] = wp_exp2(AV_RL16(buf));
                    buf += 2;
                }
            }
            got_entropy = 1;
            break;
        case WP_ID_HYBRID:
            if(s->hybrid_bitrate){
                for(i = 0; i <= s->stereo_in; i++){
                    s->ch[i].slow_level = wp_exp2(AV_RL16(buf));
                    buf += 2;
                    size -= 2;
                }
            }
            for(i = 0; i < (s->stereo_in + 1); i++){
                s->ch[i].bitrate_acc = AV_RL16(buf) << 16;
                buf += 2;
                size -= 2;
            }
            if(size > 0){
                for(i = 0; i < (s->stereo_in + 1); i++){
                    s->ch[i].bitrate_delta = wp_exp2((int16_t)AV_RL16(buf));
                    buf += 2;
                }
            }else{
                for(i = 0; i < (s->stereo_in + 1); i++)
                    s->ch[i].bitrate_delta = 0;
            }
            got_hybrid = 1;
            break;
        case WP_ID_INT32INFO:
            if(size != 4){
                av_log(avctx, AV_LOG_ERROR, "Invalid INT32INFO, size = %i, sent_bits = %i\n", size, *buf);
                buf += ssize;
                continue;
            }
            if(buf[0])
                s->extra_bits = buf[0];
            else if(buf[1])
                s->shift = buf[1];
            else if(buf[2]){
                s->and = s->or = 1;
                s->shift = buf[2];
            }else if(buf[3]){
                s->and = 1;
                s->shift = buf[3];
            }
            buf += 4;
            break;
        case WP_ID_FLOATINFO:
            if(size != 4){
                av_log(avctx, AV_LOG_ERROR, "Invalid FLOATINFO, size = %i\n", size);
                buf += ssize;
                continue;
            }
            s->float_flag = buf[0];
            s->float_shift = buf[1];
            s->float_max_exp = buf[2];
            buf += 4;
            got_float = 1;
            break;
        case WP_ID_DATA:
            s->sc.offset = buf - orig_buf;
            s->sc.size   = size * 8;
            init_get_bits(&s->gb, buf, size * 8);
            s->data_size = size * 8;
            buf += size;
            got_bs = 1;
            break;
        case WP_ID_EXTRABITS:
            if(size <= 4){
                av_log(avctx, AV_LOG_ERROR, "Invalid EXTRABITS, size = %i\n", size);
                buf += size;
                continue;
            }
            s->extra_sc.offset = buf - orig_buf;
            s->extra_sc.size   = size * 8;
            init_get_bits(&s->gb_extra_bits, buf, size * 8);
            s->crc_extra_bits = get_bits_long(&s->gb_extra_bits, 32);
            buf += size;
            s->got_extra_bits = 1;
            break;
        case WP_ID_CHANINFO:
            if(size <= 1){
                av_log(avctx, AV_LOG_ERROR, "Insufficient channel information\n");
                return -1;
            }
            chan = *buf++;
            switch(size - 2){
            case 0:
                chmask = *buf;
                break;
            case 1:
                chmask = AV_RL16(buf);
                break;
            case 2:
                chmask = AV_RL24(buf);
                break;
            case 3:
                chmask = AV_RL32(buf);
                break;
            case 5:
                chan |= (buf[1] & 0xF) << 8;
                chmask = AV_RL24(buf + 2);
                break;
            default:
                av_log(avctx, AV_LOG_ERROR, "Invalid channel info size %d\n", size);
                chan = avctx->channels;
                chmask = avctx->channel_layout;
            }
            if(chan != avctx->channels){
                av_log(avctx, AV_LOG_ERROR, "Block reports total %d channels, decoder believes it's %d channels\n",
                       chan, avctx->channels);
                return -1;
            }
            if(!avctx->channel_layout)
                avctx->channel_layout = chmask;
            buf += size - 1;
            break;
        default:
            buf += size;
        }
        if(id & WP_IDF_ODD) buf++;
    }

        if(!got_terms){
            av_log(avctx, AV_LOG_ERROR, "No block with decorrelation terms\n");
            return -1;
        }
        if(!got_weights){
            av_log(avctx, AV_LOG_ERROR, "No block with decorrelation weights\n");
            return -1;
        }
        if(!got_samples){
            av_log(avctx, AV_LOG_ERROR, "No block with decorrelation samples\n");
            return -1;
        }
        if(!got_entropy){
            av_log(avctx, AV_LOG_ERROR, "No block with entropy info\n");
            return -1;
        }
        if(s->hybrid && !got_hybrid){
            av_log(avctx, AV_LOG_ERROR, "Hybrid config not found\n");
            return -1;
        }
        if(!got_bs){
            av_log(avctx, AV_LOG_ERROR, "Packed samples not found\n");
            return -1;
        }
        if(!got_float && avctx->sample_fmt == AV_SAMPLE_FMT_FLT){
            av_log(avctx, AV_LOG_ERROR, "Float information not found\n");
            return -1;
        }
        if(s->got_extra_bits && avctx->sample_fmt != AV_SAMPLE_FMT_FLT){
            const int size = get_bits_left(&s->gb_extra_bits);
            const int wanted = s->samples * s->extra_bits << s->stereo_in;
            if(size < wanted){
                av_log(avctx, AV_LOG_ERROR, "Too small EXTRABITS\n");
                s->got_extra_bits = 0;
            }
        }

    if(s->stereo_in){
        if(avctx->sample_fmt == AV_SAMPLE_FMT_S16)
            samplecount = wv_unpack_stereo(s, &s->gb, samples, AV_SAMPLE_FMT_S16);
        else if(avctx->sample_fmt == AV_SAMPLE_FMT_S32)
            samplecount = wv_unpack_stereo(s, &s->gb, samples, AV_SAMPLE_FMT_S32);
        else
            samplecount = wv_unpack_stereo(s, &s->gb, samples, AV_SAMPLE_FMT_FLT);

        if (samplecount < 0)
            return -1;

        samplecount >>= 1;
    }else{
        const int channel_stride = avctx->channels;

        if(avctx->sample_fmt == AV_SAMPLE_FMT_S16)
            samplecount = wv_unpack_mono(s, &s->gb, samples, AV_SAMPLE_FMT_S16);
        else if(avctx->sample_fmt == AV_SAMPLE_FMT_S32)
            samplecount = wv_unpack_mono(s, &s->gb, samples, AV_SAMPLE_FMT_S32);
        else
            samplecount = wv_unpack_mono(s, &s->gb, samples, AV_SAMPLE_FMT_FLT);

        if (samplecount < 0)
            return -1;

        if(s->stereo && avctx->sample_fmt == AV_SAMPLE_FMT_S16){
            int16_t *dst = (int16_t*)samples + 1;
            int16_t *src = (int16_t*)samples;
            int cnt = samplecount;
            while(cnt--){
                *dst = *src;
                src += channel_stride;
                dst += channel_stride;
            }
        }else if(s->stereo && avctx->sample_fmt == AV_SAMPLE_FMT_S32){
            int32_t *dst = (int32_t*)samples + 1;
            int32_t *src = (int32_t*)samples;
            int cnt = samplecount;
            while(cnt--){
                *dst = *src;
                src += channel_stride;
                dst += channel_stride;
            }
        }else if(s->stereo){
            float *dst = (float*)samples + 1;
            float *src = (float*)samples;
            int cnt = samplecount;
            while(cnt--){
                *dst = *src;
                src += channel_stride;
                dst += channel_stride;
            }
        }
    }

    *got_frame_ptr = 1;

    return samplecount * bpp;
}
