static int aac_encode_frame(AVCodecContext *avctx,
                            uint8_t *frame, int buf_size, void *data)
{
    AACEncContext *s = avctx->priv_data;
    int16_t *samples = s->samples, *samples2, *la;
    ChannelElement *cpe;
    int i, j, chans, tag, start_ch;
    const uint8_t *chan_map = aac_chan_configs[avctx->channels-1];
    int chan_el_counter[4];
    FFPsyWindowInfo windows[avctx->channels];

    if (s->last_frame)
        return 0;
    if (data) {
        if (!s->psypp) {
            memcpy(s->samples + 1024 * avctx->channels, data,
                   1024 * avctx->channels * sizeof(s->samples[0]));
        } else {
            start_ch = 0;
            samples2 = s->samples + 1024 * avctx->channels;
            for (i = 0; i < chan_map[0]; i++) {
                tag = chan_map[i+1];
                chans = tag == TYPE_CPE ? 2 : 1;
                ff_psy_preprocess(s->psypp, (uint16_t*)data + start_ch,
                                  samples2 + start_ch, start_ch, chans);
                start_ch += chans;
            }
        }
    }
    if (!avctx->frame_number) {
        memcpy(s->samples, s->samples + 1024 * avctx->channels,
               1024 * avctx->channels * sizeof(s->samples[0]));
        return 0;
    }

    start_ch = 0;
    for (i = 0; i < chan_map[0]; i++) {
        FFPsyWindowInfo* wi = windows + start_ch;
        tag      = chan_map[i+1];
        chans    = tag == TYPE_CPE ? 2 : 1;
        cpe      = &s->cpe[i];
        samples2 = samples + start_ch;
        la       = samples2 + 1024 * avctx->channels + start_ch;
        if (!data)
            la = NULL;
        for (j = 0; j < chans; j++) {
            IndividualChannelStream *ics = &cpe->ch[j].ics;
            int k;
            wi[j] = ff_psy_suggest_window(&s->psy, samples2, la, start_ch + j, ics->window_sequence[0]);
            ics->window_sequence[1] = ics->window_sequence[0];
            ics->window_sequence[0] = wi[j].window_type[0];
            ics->use_kb_window[1]   = ics->use_kb_window[0];
            ics->use_kb_window[0]   = wi[j].window_shape;
            ics->num_windows        = wi[j].num_windows;
            ics->swb_sizes          = s->psy.bands    [ics->num_windows == 8];
            ics->num_swb            = s->psy.num_bands[ics->num_windows == 8];
            for (k = 0; k < ics->num_windows; k++)
                ics->group_len[k] = wi[j].grouping[k];

            s->cur_channel = start_ch + j;
            apply_window_and_mdct(avctx, s, &cpe->ch[j], samples2, j);
        }
        start_ch += chans;
    }
    init_put_bits(&s->pb, frame, buf_size*8);
    if ((avctx->frame_number & 0xFF)==1 && !(avctx->flags & CODEC_FLAG_BITEXACT))
        put_bitstream_info(avctx, s, LIBAVCODEC_IDENT);
    start_ch = 0;
    memset(chan_el_counter, 0, sizeof(chan_el_counter));
    for (i = 0; i < chan_map[0]; i++) {
        FFPsyWindowInfo* wi = windows + start_ch;
        tag      = chan_map[i+1];
        chans    = tag == TYPE_CPE ? 2 : 1;
        cpe      = &s->cpe[i];
        for (j = 0; j < chans; j++) {
            s->coder->search_for_quantizers(avctx, s, &cpe->ch[j], s->lambda);
        }
        cpe->common_window = 0;
        if (chans > 1
            && wi[0].window_type[0] == wi[1].window_type[0]
            && wi[0].window_shape   == wi[1].window_shape) {

            cpe->common_window = 1;
            for (j = 0; j < wi[0].num_windows; j++) {
                if (wi[0].grouping[j] != wi[1].grouping[j]) {
                    cpe->common_window = 0;
                    break;
                }
            }
        }
        if (cpe->common_window && s->coder->search_for_ms)
            s->coder->search_for_ms(s, cpe, s->lambda);
        adjust_frame_information(s, cpe, chans);
        put_bits(&s->pb, 3, tag);
        put_bits(&s->pb, 4, chan_el_counter[tag]++);
        if (chans == 2) {
            put_bits(&s->pb, 1, cpe->common_window);
            if (cpe->common_window) {
                put_ics_info(s, &cpe->ch[0].ics);
                encode_ms_info(&s->pb, cpe);
            }
        }
        for (j = 0; j < chans; j++) {
            s->cur_channel = start_ch + j;
            ff_psy_set_band_info(&s->psy, s->cur_channel, cpe->ch[j].coeffs, &wi[j]);
            encode_individual_channel(avctx, s, &cpe->ch[j], cpe->common_window);
        }
        start_ch += chans;
    }

    put_bits(&s->pb, 3, TYPE_END);
    flush_put_bits(&s->pb);
    avctx->frame_bits = put_bits_count(&s->pb);

    // rate control stuff
    if (!(avctx->flags & CODEC_FLAG_QSCALE)) {
        float ratio = avctx->bit_rate * 1024.0f / avctx->sample_rate / avctx->frame_bits;
        s->lambda *= ratio;
        s->lambda = fminf(s->lambda, 65536.f);
    }

    if (avctx->frame_bits > 6144*avctx->channels)
        av_log(avctx, AV_LOG_ERROR, "input buffer violation %d > %d.\n",
               avctx->frame_bits, 6144*avctx->channels);

    if (!data)
        s->last_frame = 1;
    memcpy(s->samples, s->samples + 1024 * avctx->channels,
           1024 * avctx->channels * sizeof(s->samples[0]));
    return put_bits_count(&s->pb)>>3;
}
