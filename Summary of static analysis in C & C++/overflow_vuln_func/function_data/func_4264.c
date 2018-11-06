static int read_audio_mux_element(struct LATMContext *latmctx,
                                  GetBitContext *gb)
{
    int err;
    uint8_t use_same_mux = get_bits(gb, 1);
    if (!use_same_mux) {
        if ((err = read_stream_mux_config(latmctx, gb)) < 0)
            return err;
    } else if (!latmctx->aac_ctx.avctx->extradata) {
        av_log(latmctx->aac_ctx.avctx, AV_LOG_DEBUG,
               "no decoder config found\n");
        return AVERROR(EAGAIN);
    }
    if (latmctx->audio_mux_version_A == 0) {
        int mux_slot_length_bytes = read_payload_length_info(latmctx, gb);
        if (mux_slot_length_bytes * 8 > get_bits_left(gb)) {
            av_log(latmctx->aac_ctx.avctx, AV_LOG_ERROR, "incomplete frame\n");
            return AVERROR_INVALIDDATA;
        } else if (mux_slot_length_bytes * 8 + 256 < get_bits_left(gb)) {
            av_log(latmctx->aac_ctx.avctx, AV_LOG_ERROR,
                   "frame length mismatch %d << %d\n",
                   mux_slot_length_bytes * 8, get_bits_left(gb));
            return AVERROR_INVALIDDATA;
        }
    }
    return 0;
}
