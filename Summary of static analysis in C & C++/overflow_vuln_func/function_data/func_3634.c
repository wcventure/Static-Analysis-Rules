int ff_dca_xll_filter_frame(DCAXllDecoder *s, AVFrame *frame)
{
    AVCodecContext *avctx = s->avctx;
    DCAContext *dca = avctx->priv_data;
    DCAExssAsset *asset = &dca->exss.assets[0];
    DCAXllChSet *p = &s->chset[0], *c;
    enum AVMatrixEncoding matrix_encoding = AV_MATRIX_ENCODING_NONE;
    int i, j, k, ret, shift, nsamples, request_mask;
    int ch_remap[DCA_SPEAKER_COUNT];

    // Force lossy downmixed output during recovery
    if (dca->packet & DCA_PACKET_RECOVERY) {
        for (i = 0, c = s->chset; i < s->nchsets; i++, c++) {
            if (i < s->nactivechsets)
                force_lossy_output(s, c);

            if (!c->primary_chset)
                c->dmix_embedded = 0;
        }

        s->scalable_lsbs = 0;
        s->fixed_lsb_width = 0;
    }

    // Filter frequency bands for active channel sets
    s->output_mask = 0;
    for (i = 0, c = s->chset; i < s->nactivechsets; i++, c++) {
        chs_filter_band_data(s, c, 0);

        if (c->residual_encode != (1 << c->nchannels) - 1
            && (ret = combine_residual_frame(s, c)) < 0)
            return ret;

        if (s->scalable_lsbs)
            chs_assemble_msbs_lsbs(s, c, 0);

        if (c->nfreqbands > 1) {
            chs_filter_band_data(s, c, 1);
            chs_assemble_msbs_lsbs(s, c, 1);
        }

        s->output_mask |= c->ch_mask;
    }

    // Undo hierarchial downmix and/or apply scaling
    for (i = 1, c = &s->chset[1]; i < s->nchsets; i++, c++) {
        if (!is_hier_dmix_chset(c))
            continue;

        if (i >= s->nactivechsets) {
            for (j = 0; j < c->nfreqbands; j++)
                if (c->bands[j].dmix_embedded)
                    scale_down_mix(s, c, j);
            break;
        }

        for (j = 0; j < c->nfreqbands; j++)
            if (c->bands[j].dmix_embedded)
                undo_down_mix(s, c, j);
    }

    // Assemble frequency bands for active channel sets
    if (s->nfreqbands > 1) {
        for (i = 0; i < s->nactivechsets; i++)
            if ((ret = chs_assemble_freq_bands(s, &s->chset[i])) < 0)
                return ret;
    }

    // Normalize to regular 5.1 layout if downmixing
    if (dca->request_channel_layout) {
        if (s->output_mask & DCA_SPEAKER_MASK_Lss) {
            s->output_samples[DCA_SPEAKER_Ls] = s->output_samples[DCA_SPEAKER_Lss];
            s->output_mask = (s->output_mask & ~DCA_SPEAKER_MASK_Lss) | DCA_SPEAKER_MASK_Ls;
        }
        if (s->output_mask & DCA_SPEAKER_MASK_Rss) {
            s->output_samples[DCA_SPEAKER_Rs] = s->output_samples[DCA_SPEAKER_Rss];
            s->output_mask = (s->output_mask & ~DCA_SPEAKER_MASK_Rss) | DCA_SPEAKER_MASK_Rs;
        }
    }

    // Handle downmixing to stereo request
    if (dca->request_channel_layout == DCA_SPEAKER_LAYOUT_STEREO
        && DCA_HAS_STEREO(s->output_mask) && p->dmix_embedded
        && (p->dmix_type == DCA_DMIX_TYPE_LoRo ||
            p->dmix_type == DCA_DMIX_TYPE_LtRt))
        request_mask = DCA_SPEAKER_LAYOUT_STEREO;
    else
        request_mask = s->output_mask;
    if (!ff_dca_set_channel_layout(avctx, ch_remap, request_mask))
        return AVERROR(EINVAL);

    avctx->sample_rate = p->freq << (s->nfreqbands - 1);

    switch (p->storage_bit_res) {
    case 16:
        avctx->sample_fmt = AV_SAMPLE_FMT_S16P;
        shift = 16 - p->pcm_bit_res;
        break;
    case 20:
    case 24:
        avctx->sample_fmt = AV_SAMPLE_FMT_S32P;
        shift = 24 - p->pcm_bit_res;
        break;
    default:
        return AVERROR(EINVAL);
    }

    avctx->bits_per_raw_sample = p->storage_bit_res;
    avctx->profile = FF_PROFILE_DTS_HD_MA;
    avctx->bit_rate = 0;

    frame->nb_samples = nsamples = s->nframesamples << (s->nfreqbands - 1);
    if ((ret = ff_get_buffer(avctx, frame, 0)) < 0)
        return ret;

    // Downmix primary channel set to stereo
    if (request_mask != s->output_mask) {
        ff_dca_downmix_to_stereo_fixed(s->dcadsp, s->output_samples,
                                       p->dmix_coeff, nsamples,
                                       s->output_mask);
    }

    for (i = 0; i < avctx->channels; i++) {
        int32_t *samples = s->output_samples[ch_remap[i]];
        if (frame->format == AV_SAMPLE_FMT_S16P) {
            int16_t *plane = (int16_t *)frame->extended_data[i];
            for (k = 0; k < nsamples; k++)
                plane[k] = av_clip_int16(samples[k] * (1 << shift));
        } else {
            int32_t *plane = (int32_t *)frame->extended_data[i];
            for (k = 0; k < nsamples; k++)
                plane[k] = clip23(samples[k] * (1 << shift)) * (1 << 8);
        }
    }

    if (!asset->one_to_one_map_ch_to_spkr) {
        if (asset->representation_type == DCA_REPR_TYPE_LtRt)
            matrix_encoding = AV_MATRIX_ENCODING_DOLBY;
        else if (asset->representation_type == DCA_REPR_TYPE_LhRh)
            matrix_encoding = AV_MATRIX_ENCODING_DOLBYHEADPHONE;
    } else if (request_mask != s->output_mask && p->dmix_type == DCA_DMIX_TYPE_LtRt) {
        matrix_encoding = AV_MATRIX_ENCODING_DOLBY;
    }
    if ((ret = ff_side_data_update_matrix_encoding(frame, matrix_encoding)) < 0)
        return ret;

    return 0;
}
