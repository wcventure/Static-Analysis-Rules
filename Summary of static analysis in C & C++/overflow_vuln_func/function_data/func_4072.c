static int decode_residuals(FLACContext *s, int32_t *decoded, int pred_order)
{
    int i, tmp, partition, method_type, rice_order;
    int rice_bits, rice_esc;
    int samples;

    method_type = get_bits(&s->gb, 2);
    if (method_type > 1) {
        av_log(s->avctx, AV_LOG_ERROR, "illegal residual coding method %d\n",
               method_type);
        return AVERROR_INVALIDDATA;
    }

    rice_order = get_bits(&s->gb, 4);

    samples= s->blocksize >> rice_order;
    if (samples << rice_order != s->blocksize) {
        av_log(s->avctx, AV_LOG_ERROR, "invalid rice order: %i blocksize %i\n",
               rice_order, s->blocksize);
        return AVERROR_INVALIDDATA;
    }

    if (pred_order > samples) {
        av_log(s->avctx, AV_LOG_ERROR, "invalid predictor order: %i > %i\n",
               pred_order, samples);
        return AVERROR_INVALIDDATA;
    }

    rice_bits = 4 + method_type;
    rice_esc  = (1 << rice_bits) - 1;

    decoded += pred_order;
    i= pred_order;
    for (partition = 0; partition < (1 << rice_order); partition++) {
        tmp = get_bits(&s->gb, rice_bits);
        if (tmp == rice_esc) {
            tmp = get_bits(&s->gb, 5);
            for (; i < samples; i++)
                *decoded++ = get_sbits_long(&s->gb, tmp);
        } else {
            for (; i < samples; i++) {
                int v = get_sr_golomb_flac(&s->gb, tmp, INT_MAX, 0);
                if (v == 0x80000000){
                    av_log(s->avctx, AV_LOG_ERROR, "invalid residual\n");
                    return AVERROR_INVALIDDATA;
                }

                *decoded++ = v;
            }
        }
        i= 0;
    }

    return 0;
}
