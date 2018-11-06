static int ac3_encode_frame(AVCodecContext *avctx, unsigned char *frame,
                            int buf_size, void *data)
{
    AC3EncodeContext *s = avctx->priv_data;
    const SampleType *samples = data;
    int ret;

    if (s->bit_alloc.sr_code == 1)
        adjust_frame_size(s);

    deinterleave_input_samples(s, samples);

    apply_mdct(s);

    compute_rematrixing_strategy(s);

    scale_coefficients(s);

    apply_rematrixing(s);

    process_exponents(s);

    ret = compute_bit_allocation(s);
    if (ret) {
        av_log(avctx, AV_LOG_ERROR, "Bit allocation failed. Try increasing the bitrate.\n");
        return ret;
    }

    quantize_mantissas(s);

    output_frame(s, frame);

    return s->frame_size;
}
