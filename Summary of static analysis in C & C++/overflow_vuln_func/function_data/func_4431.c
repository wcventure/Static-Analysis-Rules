int ff_hevc_cu_qp_delta_abs(HEVCContext *s)
{
    int prefix_val = 0;
    int suffix_val = 0;
    int inc = 0;

    while (prefix_val < 5 && GET_CABAC(elem_offset[CU_QP_DELTA] + inc)) {
        prefix_val++;
        inc = 1;
    }
    if (prefix_val >= 5) {
        int k = 0;
        while (k < CABAC_MAX_BIN && get_cabac_bypass(&s->HEVClc->cc)) {
            suffix_val += 1 << k;
            k++;
        }
        if (k == CABAC_MAX_BIN)
            av_log(s->avctx, AV_LOG_ERROR, "CABAC_MAX_BIN : %d\n", k);

        while (k--)
            suffix_val += get_cabac_bypass(&s->HEVClc->cc) << k;
    }
    return prefix_val + suffix_val;
}
