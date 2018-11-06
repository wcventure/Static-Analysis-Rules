static int allocate_buffers(ShortenContext *s)
{
    int i, chan, err;

    for (chan = 0; chan < s->channels; chan++) {
        if (FFMAX(1, s->nmean) >= UINT_MAX / sizeof(int32_t)) {
            av_log(s->avctx, AV_LOG_ERROR, "nmean too large\n");
            return AVERROR_INVALIDDATA;
        }
        if (s->blocksize + s->nwrap >= UINT_MAX / sizeof(int32_t) ||
            s->blocksize + s->nwrap <= (unsigned)s->nwrap) {
            av_log(s->avctx, AV_LOG_ERROR,
                   "s->blocksize + s->nwrap too large\n");
            return AVERROR_INVALIDDATA;
        }

        if ((err = av_reallocp_array(&s->offset[chan],
                               sizeof(int32_t),
                               FFMAX(1, s->nmean))) < 0)
            return err;

        if ((err = av_reallocp_array(&s->decoded_base[chan], (s->blocksize + s->nwrap),
                               sizeof(s->decoded_base[0][0]))) < 0)
            return err;
        for (i = 0; i < s->nwrap; i++)
            s->decoded_base[chan][i] = 0;
        s->decoded[chan] = s->decoded_base[chan] + s->nwrap;
    }

    if ((err = av_reallocp_array(&s->coeffs, s->nwrap, sizeof(*s->coeffs))) < 0)
        return err;

    return 0;
}
