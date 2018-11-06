static av_always_inline int coeff_abs_level_remaining_decode(HEVCContext *s, int rc_rice_param)
{
    int prefix = 0;
    int suffix = 0;
    int last_coeff_abs_level_remaining;
    int i;

    while (prefix < CABAC_MAX_BIN && get_cabac_bypass(&s->HEVClc->cc))
        prefix++;
    if (prefix == CABAC_MAX_BIN)
        av_log(s->avctx, AV_LOG_ERROR, "CABAC_MAX_BIN : %d\n", prefix);
    if (prefix < 3) {
        for (i = 0; i < rc_rice_param; i++)
            suffix = (suffix << 1) | get_cabac_bypass(&s->HEVClc->cc);
        last_coeff_abs_level_remaining = (prefix << rc_rice_param) + suffix;
    } else {
        int prefix_minus3 = prefix - 3;
        for (i = 0; i < prefix_minus3 + rc_rice_param; i++)
            suffix = (suffix << 1) | get_cabac_bypass(&s->HEVClc->cc);
        last_coeff_abs_level_remaining = (((1 << prefix_minus3) + 3 - 1)
                                              << rc_rice_param) + suffix;
    }
    return last_coeff_abs_level_remaining;
}
