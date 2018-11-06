static av_cold int mpc8_decode_init(AVCodecContext * avctx)
{
    int i;
    MPCContext *c = avctx->priv_data;
    GetBitContext gb;
    static int vlc_initialized = 0;
    int channels;

    static VLC_TYPE band_table[542][2];
    static VLC_TYPE q1_table[520][2];
    static VLC_TYPE q9up_table[524][2];
    static VLC_TYPE scfi0_table[1 << MPC8_SCFI0_BITS][2];
    static VLC_TYPE scfi1_table[1 << MPC8_SCFI1_BITS][2];
    static VLC_TYPE dscf0_table[560][2];
    static VLC_TYPE dscf1_table[598][2];
    static VLC_TYPE q3_0_table[512][2];
    static VLC_TYPE q3_1_table[516][2];
    static VLC_TYPE codes_table[5708][2];

    if(avctx->extradata_size < 2){
        av_log(avctx, AV_LOG_ERROR, "Too small extradata size (%i)!\n", avctx->extradata_size);
        return -1;
    }
    memset(c->oldDSCF, 0, sizeof(c->oldDSCF));
    av_lfg_init(&c->rnd, 0xDEADBEEF);
    ff_dsputil_init(&c->dsp, avctx);
    ff_mpadsp_init(&c->mpadsp);

    ff_mpc_init();

    init_get_bits(&gb, avctx->extradata, 16);

    skip_bits(&gb, 3);//sample rate
    c->maxbands = get_bits(&gb, 5) + 1;
    if (c->maxbands >= BANDS) {
        av_log(avctx,AV_LOG_ERROR, "maxbands %d too high\n", c->maxbands);
        return AVERROR_INVALIDDATA;
    }
    channels = get_bits(&gb, 4) + 1;
    if (channels > 2) {
        av_log_missing_feature(avctx, "Multichannel MPC SV8", 1);
        return -1;
    }
    c->MSS = get_bits1(&gb);
    c->frames = 1 << (get_bits(&gb, 3) * 2);

    avctx->sample_fmt = AV_SAMPLE_FMT_S16;
    avctx->channel_layout = (avctx->channels==2) ? AV_CH_LAYOUT_STEREO : AV_CH_LAYOUT_MONO;

    if(vlc_initialized) return 0;
    av_log(avctx, AV_LOG_DEBUG, "Initing VLC\n");

    band_vlc.table = band_table;
    band_vlc.table_allocated = 542;
    init_vlc(&band_vlc, MPC8_BANDS_BITS, MPC8_BANDS_SIZE,
             mpc8_bands_bits,  1, 1,
             mpc8_bands_codes, 1, 1, INIT_VLC_USE_NEW_STATIC);

    q1_vlc.table = q1_table;
    q1_vlc.table_allocated = 520;
    init_vlc(&q1_vlc, MPC8_Q1_BITS, MPC8_Q1_SIZE,
             mpc8_q1_bits,  1, 1,
             mpc8_q1_codes, 1, 1, INIT_VLC_USE_NEW_STATIC);
    q9up_vlc.table = q9up_table;
    q9up_vlc.table_allocated = 524;
    init_vlc(&q9up_vlc, MPC8_Q9UP_BITS, MPC8_Q9UP_SIZE,
             mpc8_q9up_bits,  1, 1,
             mpc8_q9up_codes, 1, 1, INIT_VLC_USE_NEW_STATIC);

    scfi_vlc[0].table = scfi0_table;
    scfi_vlc[0].table_allocated = 1 << MPC8_SCFI0_BITS;
    init_vlc(&scfi_vlc[0], MPC8_SCFI0_BITS, MPC8_SCFI0_SIZE,
             mpc8_scfi0_bits,  1, 1,
             mpc8_scfi0_codes, 1, 1, INIT_VLC_USE_NEW_STATIC);
    scfi_vlc[1].table = scfi1_table;
    scfi_vlc[1].table_allocated = 1 << MPC8_SCFI1_BITS;
    init_vlc(&scfi_vlc[1], MPC8_SCFI1_BITS, MPC8_SCFI1_SIZE,
             mpc8_scfi1_bits,  1, 1,
             mpc8_scfi1_codes, 1, 1, INIT_VLC_USE_NEW_STATIC);

    dscf_vlc[0].table = dscf0_table;
    dscf_vlc[0].table_allocated = 560;
    init_vlc(&dscf_vlc[0], MPC8_DSCF0_BITS, MPC8_DSCF0_SIZE,
             mpc8_dscf0_bits,  1, 1,
             mpc8_dscf0_codes, 1, 1, INIT_VLC_USE_NEW_STATIC);
    dscf_vlc[1].table = dscf1_table;
    dscf_vlc[1].table_allocated = 598;
    init_vlc(&dscf_vlc[1], MPC8_DSCF1_BITS, MPC8_DSCF1_SIZE,
             mpc8_dscf1_bits,  1, 1,
             mpc8_dscf1_codes, 1, 1, INIT_VLC_USE_NEW_STATIC);

    q3_vlc[0].table = q3_0_table;
    q3_vlc[0].table_allocated = 512;
    ff_init_vlc_sparse(&q3_vlc[0], MPC8_Q3_BITS, MPC8_Q3_SIZE,
             mpc8_q3_bits,  1, 1,
             mpc8_q3_codes, 1, 1,
             mpc8_q3_syms,  1, 1, INIT_VLC_USE_NEW_STATIC);
    q3_vlc[1].table = q3_1_table;
    q3_vlc[1].table_allocated = 516;
    ff_init_vlc_sparse(&q3_vlc[1], MPC8_Q4_BITS, MPC8_Q4_SIZE,
             mpc8_q4_bits,  1, 1,
             mpc8_q4_codes, 1, 1,
             mpc8_q4_syms,  1, 1, INIT_VLC_USE_NEW_STATIC);

    for(i = 0; i < 2; i++){
        res_vlc[i].table = &codes_table[vlc_offsets[0+i]];
        res_vlc[i].table_allocated = vlc_offsets[1+i] - vlc_offsets[0+i];
        init_vlc(&res_vlc[i], MPC8_RES_BITS, MPC8_RES_SIZE,
                 &mpc8_res_bits[i],  1, 1,
                 &mpc8_res_codes[i], 1, 1, INIT_VLC_USE_NEW_STATIC);

        q2_vlc[i].table = &codes_table[vlc_offsets[2+i]];
        q2_vlc[i].table_allocated = vlc_offsets[3+i] - vlc_offsets[2+i];
        init_vlc(&q2_vlc[i], MPC8_Q2_BITS, MPC8_Q2_SIZE,
                 &mpc8_q2_bits[i],  1, 1,
                 &mpc8_q2_codes[i], 1, 1, INIT_VLC_USE_NEW_STATIC);

        quant_vlc[0][i].table = &codes_table[vlc_offsets[4+i]];
        quant_vlc[0][i].table_allocated = vlc_offsets[5+i] - vlc_offsets[4+i];
        init_vlc(&quant_vlc[0][i], MPC8_Q5_BITS, MPC8_Q5_SIZE,
                 &mpc8_q5_bits[i],  1, 1,
                 &mpc8_q5_codes[i], 1, 1, INIT_VLC_USE_NEW_STATIC);
        quant_vlc[1][i].table = &codes_table[vlc_offsets[6+i]];
        quant_vlc[1][i].table_allocated = vlc_offsets[7+i] - vlc_offsets[6+i];
        init_vlc(&quant_vlc[1][i], MPC8_Q6_BITS, MPC8_Q6_SIZE,
                 &mpc8_q6_bits[i],  1, 1,
                 &mpc8_q6_codes[i], 1, 1, INIT_VLC_USE_NEW_STATIC);
        quant_vlc[2][i].table = &codes_table[vlc_offsets[8+i]];
        quant_vlc[2][i].table_allocated = vlc_offsets[9+i] - vlc_offsets[8+i];
        init_vlc(&quant_vlc[2][i], MPC8_Q7_BITS, MPC8_Q7_SIZE,
                 &mpc8_q7_bits[i],  1, 1,
                 &mpc8_q7_codes[i], 1, 1, INIT_VLC_USE_NEW_STATIC);
        quant_vlc[3][i].table = &codes_table[vlc_offsets[10+i]];
        quant_vlc[3][i].table_allocated = vlc_offsets[11+i] - vlc_offsets[10+i];
        init_vlc(&quant_vlc[3][i], MPC8_Q8_BITS, MPC8_Q8_SIZE,
                 &mpc8_q8_bits[i],  1, 1,
                 &mpc8_q8_codes[i], 1, 1, INIT_VLC_USE_NEW_STATIC);
    }
    vlc_initialized = 1;

    avcodec_get_frame_defaults(&c->frame);
    avctx->coded_frame = &c->frame;

    return 0;
}
