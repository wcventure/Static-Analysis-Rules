static av_always_inline int mvd_decode(HEVCContext *s)
{
    int ret = 2;
    int k = 1;

    while (k < CABAC_MAX_BIN && get_cabac_bypass(&s->HEVClc->cc)) {
        ret += 1 << k;
        k++;
    }
    if (k == CABAC_MAX_BIN)
        av_log(s->avctx, AV_LOG_ERROR, "CABAC_MAX_BIN : %d\n", k);
    while (k--)
        ret += get_cabac_bypass(&s->HEVClc->cc) << k;
    return get_cabac_bypass_sign(&s->HEVClc->cc, -ret);
}
