static int mpc7_decode_frame(AVCodecContext * avctx, void *data,
                             int *got_frame_ptr, AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size;
    MPCContext *c = avctx->priv_data;
    GetBitContext gb;
    int i, ch;
    int mb = -1;
    Band *bands = c->bands;
    int off, ret, last_frame, skip;
    int bits_used, bits_avail;

    memset(bands, 0, sizeof(*bands) * (c->maxbands + 1));

    buf_size = avpkt->size & ~3;
    if (buf_size <= 0) {
        av_log(avctx, AV_LOG_ERROR, "packet size is too small (%i bytes)\n",
               avpkt->size);
        return AVERROR_INVALIDDATA;
    }
    if (buf_size != avpkt->size) {
        av_log(avctx, AV_LOG_WARNING, "packet size is not a multiple of 4. "
               "extra bytes at the end will be skipped.\n");
    }

    skip       = buf[0];
    last_frame = buf[1];
    buf       += 4;
    buf_size  -= 4;

    /
    c->frame.nb_samples = last_frame ? c->lastframelen : MPC_FRAME_SIZE;
    if ((ret = avctx->get_buffer(avctx, &c->frame)) < 0) {
        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");
        return ret;
    }

    av_fast_padded_malloc(&c->bits, &c->buf_size, buf_size);
    if (!c->bits)
        return AVERROR(ENOMEM);
    c->dsp.bswap_buf((uint32_t *)c->bits, (const uint32_t *)buf, buf_size >> 2);
    init_get_bits(&gb, c->bits, buf_size * 8);
    skip_bits_long(&gb, skip);

    /
    for(i = 0; i <= c->maxbands; i++){
        for(ch = 0; ch < 2; ch++){
            int t = 4;
            if(i) t = get_vlc2(&gb, hdr_vlc.table, MPC7_HDR_BITS, 1) - 5;
            if(t == 4) bands[i].res[ch] = get_bits(&gb, 4);
            else bands[i].res[ch] = bands[i-1].res[ch] + t;
            if (bands[i].res[ch] < -1 || bands[i].res[ch] > 17) {
                av_log(avctx, AV_LOG_ERROR, "subband index invalid\n");
                return AVERROR_INVALIDDATA;
            }
        }

        if(bands[i].res[0] || bands[i].res[1]){
            mb = i;
            if(c->MSS) bands[i].msf = get_bits1(&gb);
        }
    }
    /
    for(i = 0; i <= mb; i++)
        for(ch = 0; ch < 2; ch++)
            if(bands[i].res[ch]) bands[i].scfi[ch] = get_vlc2(&gb, scfi_vlc.table, MPC7_SCFI_BITS, 1);
    /
    for(i = 0; i <= mb; i++){
        for(ch = 0; ch < 2; ch++){
            if(bands[i].res[ch]){
                bands[i].scf_idx[ch][2] = c->oldDSCF[ch][i];
                bands[i].scf_idx[ch][0] = get_scale_idx(&gb, bands[i].scf_idx[ch][2]);
                switch(bands[i].scfi[ch]){
                case 0:
                    bands[i].scf_idx[ch][1] = get_scale_idx(&gb, bands[i].scf_idx[ch][0]);
                    bands[i].scf_idx[ch][2] = get_scale_idx(&gb, bands[i].scf_idx[ch][1]);
                    break;
                case 1:
                    bands[i].scf_idx[ch][1] = get_scale_idx(&gb, bands[i].scf_idx[ch][0]);
                    bands[i].scf_idx[ch][2] = bands[i].scf_idx[ch][1];
                    break;
                case 2:
                    bands[i].scf_idx[ch][1] = bands[i].scf_idx[ch][0];
                    bands[i].scf_idx[ch][2] = get_scale_idx(&gb, bands[i].scf_idx[ch][1]);
                    break;
                case 3:
                    bands[i].scf_idx[ch][2] = bands[i].scf_idx[ch][1] = bands[i].scf_idx[ch][0];
                    break;
                }
                c->oldDSCF[ch][i] = bands[i].scf_idx[ch][2];
            }
        }
    }
    /
    memset(c->Q, 0, sizeof(c->Q));
    off = 0;
    for(i = 0; i < BANDS; i++, off += SAMPLES_PER_BAND)
        for(ch = 0; ch < 2; ch++)
            idx_to_quant(c, &gb, bands[i].res[ch], c->Q[ch] + off);

    ff_mpc_dequantize_and_synth(c, mb, c->frame.data[0], 2);

    bits_used = get_bits_count(&gb);
    bits_avail = buf_size * 8;
    if (!last_frame && ((bits_avail < bits_used) || (bits_used + 32 <= bits_avail))) {
        av_log(avctx, AV_LOG_ERROR, "Error decoding frame: used %i of %i bits\n", bits_used, bits_avail);
        return -1;
    }
    if(c->frames_to_skip){
        c->frames_to_skip--;
        *got_frame_ptr = 0;
        return avpkt->size;
    }

    *got_frame_ptr   = 1;
    *(AVFrame *)data = c->frame;

    return avpkt->size;
}
