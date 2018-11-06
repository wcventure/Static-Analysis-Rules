static av_cold int decode_end(AVCodecContext *avctx)
{
    ALSDecContext *ctx = avctx->priv_data;

    av_freep(&ctx->sconf.chan_pos);

    ff_bgmc_end(&ctx->bgmc_lut, &ctx->bgmc_lut_status);

    av_freep(&ctx->const_block);
    av_freep(&ctx->shift_lsbs);
    av_freep(&ctx->opt_order);
    av_freep(&ctx->store_prev_samples);
    av_freep(&ctx->use_ltp);
    av_freep(&ctx->ltp_lag);
    av_freep(&ctx->ltp_gain);
    av_freep(&ctx->ltp_gain_buffer);
    av_freep(&ctx->quant_cof);
    av_freep(&ctx->lpc_cof);
    av_freep(&ctx->quant_cof_buffer);
    av_freep(&ctx->lpc_cof_buffer);
    av_freep(&ctx->lpc_cof_reversed_buffer);
    av_freep(&ctx->prev_raw_samples);
    av_freep(&ctx->raw_samples);
    av_freep(&ctx->raw_buffer);
    av_freep(&ctx->chan_data);
    av_freep(&ctx->chan_data_buffer);
    av_freep(&ctx->reverted_channels);
    av_freep(&ctx->crc_buffer);
    av_freep(&ctx->mlz);
    av_freep(&ctx->acf);
    av_freep(&ctx->last_acf_mantissa);
    av_freep(&ctx->shift_value);
    av_freep(&ctx->last_shift_value);
    av_freep(&ctx->raw_mantissa);
    av_freep(&ctx->larray);
    av_freep(&ctx->nbits);

    return 0;
}
