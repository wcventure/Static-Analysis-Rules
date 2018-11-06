static av_cold int read_specific_config(ALSDecContext *ctx)
{
    GetBitContext gb;
    uint64_t ht_size;
    int i, config_offset;
    MPEG4AudioConfig m4ac;
    ALSSpecificConfig *sconf = &ctx->sconf;
    AVCodecContext *avctx    = ctx->avctx;
    uint32_t als_id, header_size, trailer_size;

    init_get_bits(&gb, avctx->extradata, avctx->extradata_size * 8);

    config_offset = avpriv_mpeg4audio_get_config(&m4ac, avctx->extradata,
                                                 avctx->extradata_size * 8, 1);

    if (config_offset < 0)
        return -1;

    skip_bits_long(&gb, config_offset);

    if (get_bits_left(&gb) < (30 << 3))
        return -1;

    // read the fixed items
    als_id                      = get_bits_long(&gb, 32);
    avctx->sample_rate          = m4ac.sample_rate;
    skip_bits_long(&gb, 32); // sample rate already known
    sconf->samples              = get_bits_long(&gb, 32);
    avctx->channels             = m4ac.channels;
    skip_bits(&gb, 16);      // number of channels already known
    skip_bits(&gb, 3);       // skip file_type
    sconf->resolution           = get_bits(&gb, 3);
    sconf->floating             = get_bits1(&gb);
    sconf->msb_first            = get_bits1(&gb);
    sconf->frame_length         = get_bits(&gb, 16) + 1;
    sconf->ra_distance          = get_bits(&gb, 8);
    sconf->ra_flag              = get_bits(&gb, 2);
    sconf->adapt_order          = get_bits1(&gb);
    sconf->coef_table           = get_bits(&gb, 2);
    sconf->long_term_prediction = get_bits1(&gb);
    sconf->max_order            = get_bits(&gb, 10);
    sconf->block_switching      = get_bits(&gb, 2);
    sconf->bgmc                 = get_bits1(&gb);
    sconf->sb_part              = get_bits1(&gb);
    sconf->joint_stereo         = get_bits1(&gb);
    sconf->mc_coding            = get_bits1(&gb);
    sconf->chan_config          = get_bits1(&gb);
    sconf->chan_sort            = get_bits1(&gb);
    sconf->crc_enabled          = get_bits1(&gb);
    sconf->rlslms               = get_bits1(&gb);
    skip_bits(&gb, 5);       // skip 5 reserved bits
    skip_bits1(&gb);         // skip aux_data_enabled


    // check for ALSSpecificConfig struct
    if (als_id != MKBETAG('A','L','S','\0'))
        return -1;

    ctx->cur_frame_length = sconf->frame_length;

    // read channel config
    if (sconf->chan_config)
        sconf->chan_config_info = get_bits(&gb, 16);
    // TODO: use this to set avctx->channel_layout


    // read channel sorting
    if (sconf->chan_sort && avctx->channels > 1) {
        int chan_pos_bits = av_ceil_log2(avctx->channels);
        int bits_needed  = avctx->channels * chan_pos_bits + 7;
        if (get_bits_left(&gb) < bits_needed)
            return -1;

        if (!(sconf->chan_pos = av_malloc(avctx->channels * sizeof(*sconf->chan_pos))))
            return AVERROR(ENOMEM);

        ctx->cs_switch = 1;

        for (i = 0; i < avctx->channels; i++) {
            int idx;

            idx = get_bits(&gb, chan_pos_bits);
            if (idx >= avctx->channels) {
                av_log(avctx, AV_LOG_WARNING, "Invalid channel reordering.\n");
                ctx->cs_switch = 0;
                break;
            }
            sconf->chan_pos[idx] = i;
        }

        align_get_bits(&gb);
    }


    // read fixed header and trailer sizes,
    // if size = 0xFFFFFFFF then there is no data field!
    if (get_bits_left(&gb) < 64)
        return -1;

    header_size  = get_bits_long(&gb, 32);
    trailer_size = get_bits_long(&gb, 32);
    if (header_size  == 0xFFFFFFFF)
        header_size  = 0;
    if (trailer_size == 0xFFFFFFFF)
        trailer_size = 0;

    ht_size = ((int64_t)(header_size) + (int64_t)(trailer_size)) << 3;


    // skip the header and trailer data
    if (get_bits_left(&gb) < ht_size)
        return -1;

    if (ht_size > INT32_MAX)
        return -1;

    skip_bits_long(&gb, ht_size);


    // initialize CRC calculation
    if (sconf->crc_enabled) {
        if (get_bits_left(&gb) < 32)
            return -1;

        if (avctx->err_recognition & (AV_EF_CRCCHECK|AV_EF_CAREFUL)) {
            ctx->crc_table = av_crc_get_table(AV_CRC_32_IEEE_LE);
            ctx->crc       = 0xFFFFFFFF;
            ctx->crc_org   = ~get_bits_long(&gb, 32);
        } else
            skip_bits_long(&gb, 32);
    }


    // no need to read the rest of ALSSpecificConfig (ra_unit_size & aux data)

    dprint_specific_config(ctx);

    return 0;
}
