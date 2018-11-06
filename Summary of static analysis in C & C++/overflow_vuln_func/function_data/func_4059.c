static int decode_ics_info(AACContext *ac, IndividualChannelStream *ics,
                           GetBitContext *gb)
{
    const MPEG4AudioConfig *const m4ac = &ac->oc[1].m4ac;
    const int aot = m4ac->object_type;
    const int sampling_index = m4ac->sampling_index;
    if (aot != AOT_ER_AAC_ELD) {
        if (get_bits1(gb)) {
            av_log(ac->avctx, AV_LOG_ERROR, "Reserved bit set.\n");
            if (ac->avctx->err_recognition & AV_EF_BITSTREAM)
                return AVERROR_INVALIDDATA;
        }
        ics->window_sequence[1] = ics->window_sequence[0];
        ics->window_sequence[0] = get_bits(gb, 2);
        if (aot == AOT_ER_AAC_LD &&
            ics->window_sequence[0] != ONLY_LONG_SEQUENCE) {
            av_log(ac->avctx, AV_LOG_ERROR,
                   "AAC LD is only defined for ONLY_LONG_SEQUENCE but "
                   "window sequence %d found.\n", ics->window_sequence[0]);
            ics->window_sequence[0] = ONLY_LONG_SEQUENCE;
            return AVERROR_INVALIDDATA;
        }
        ics->use_kb_window[1]   = ics->use_kb_window[0];
        ics->use_kb_window[0]   = get_bits1(gb);
    }
    ics->num_window_groups  = 1;
    ics->group_len[0]       = 1;
    if (ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE) {
        int i;
        ics->max_sfb = get_bits(gb, 4);
        for (i = 0; i < 7; i++) {
            if (get_bits1(gb)) {
                ics->group_len[ics->num_window_groups - 1]++;
            } else {
                ics->num_window_groups++;
                ics->group_len[ics->num_window_groups - 1] = 1;
            }
        }
        ics->num_windows       = 8;
        ics->swb_offset        =    ff_swb_offset_128[sampling_index];
        ics->num_swb           =   ff_aac_num_swb_128[sampling_index];
        ics->tns_max_bands     = ff_tns_max_bands_128[sampling_index];
        ics->predictor_present = 0;
    } else {
        ics->max_sfb           = get_bits(gb, 6);
        ics->num_windows       = 1;
        if (aot == AOT_ER_AAC_LD || aot == AOT_ER_AAC_ELD) {
            if (m4ac->frame_length_short) {
                ics->swb_offset    =     ff_swb_offset_480[sampling_index];
                ics->num_swb       =    ff_aac_num_swb_480[sampling_index];
                ics->tns_max_bands =  ff_tns_max_bands_480[sampling_index];
            } else {
                ics->swb_offset    =     ff_swb_offset_512[sampling_index];
                ics->num_swb       =    ff_aac_num_swb_512[sampling_index];
                ics->tns_max_bands =  ff_tns_max_bands_512[sampling_index];
            }
            if (!ics->num_swb || !ics->swb_offset)
                return AVERROR_BUG;
        } else {
            ics->swb_offset    =    ff_swb_offset_1024[sampling_index];
            ics->num_swb       =   ff_aac_num_swb_1024[sampling_index];
            ics->tns_max_bands = ff_tns_max_bands_1024[sampling_index];
        }
        if (aot != AOT_ER_AAC_ELD) {
            ics->predictor_present     = get_bits1(gb);
            ics->predictor_reset_group = 0;
        }
        if (ics->predictor_present) {
            if (aot == AOT_AAC_MAIN) {
                if (decode_prediction(ac, ics, gb)) {
                    goto fail;
                }
            } else if (aot == AOT_AAC_LC ||
                       aot == AOT_ER_AAC_LC) {
                av_log(ac->avctx, AV_LOG_ERROR,
                       "Prediction is not allowed in AAC-LC.\n");
                goto fail;
            } else {
                if (aot == AOT_ER_AAC_LD) {
                    av_log(ac->avctx, AV_LOG_ERROR,
                           "LTP in ER AAC LD not yet implemented.\n");
                    return AVERROR_PATCHWELCOME;
                }
                if ((ics->ltp.present = get_bits(gb, 1)))
                    decode_ltp(&ics->ltp, gb, ics->max_sfb);
            }
        }
    }

    if (ics->max_sfb > ics->num_swb) {
        av_log(ac->avctx, AV_LOG_ERROR,
               "Number of scalefactor bands in group (%d) "
               "exceeds limit (%d).\n",
               ics->max_sfb, ics->num_swb);
        goto fail;
    }

    return 0;
fail:
    ics->max_sfb = 0;
    return AVERROR_INVALIDDATA;
}
