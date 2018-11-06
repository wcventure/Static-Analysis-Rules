static int decode_subframe_fixed(FLACContext *s, int32_t *decoded,
                                 int pred_order, int bps)
{
    const int blocksize = s->blocksize;
    int av_uninit(a), av_uninit(b), av_uninit(c), av_uninit(d), i;
    int ret;

    /
    for (i = 0; i < pred_order; i++) {
        decoded[i] = get_sbits_long(&s->gb, bps);
    }

    if ((ret = decode_residuals(s, decoded, pred_order)) < 0)
        return ret;

    if (pred_order > 0)
        a = decoded[pred_order-1];
    if (pred_order > 1)
        b = a - decoded[pred_order-2];
    if (pred_order > 2)
        c = b - decoded[pred_order-2] + decoded[pred_order-3];
    if (pred_order > 3)
        d = c - decoded[pred_order-2] + 2*decoded[pred_order-3] - decoded[pred_order-4];

    switch (pred_order) {
    case 0:
        break;
    case 1:
        for (i = pred_order; i < blocksize; i++)
            decoded[i] = a += decoded[i];
        break;
    case 2:
        for (i = pred_order; i < blocksize; i++)
            decoded[i] = a += b += decoded[i];
        break;
    case 3:
        for (i = pred_order; i < blocksize; i++)
            decoded[i] = a += b += c += decoded[i];
        break;
    case 4:
        for (i = pred_order; i < blocksize; i++)
            decoded[i] = a += b += c += d += decoded[i];
        break;
    default:
        av_log(s->avctx, AV_LOG_ERROR, "illegal pred order %d\n", pred_order);
        return AVERROR_INVALIDDATA;
    }

    return 0;
}
