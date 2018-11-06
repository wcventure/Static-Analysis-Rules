static uint32_t find_subframe_rice_params(FlacEncodeContext *s,
                                          FlacSubframe *sub, int pred_order)
{
    int pmin = get_max_p_order(s->options.min_partition_order,
                               s->frame.blocksize, pred_order);
    int pmax = get_max_p_order(s->options.max_partition_order,
                               s->frame.blocksize, pred_order);

    uint32_t bits = 8 + pred_order * sub->obits + 2 + 4;
    if (sub->type == FLAC_SUBFRAME_LPC)
        bits += 4 + 5 + pred_order * s->options.lpc_coeff_precision;
    bits += calc_rice_params(&sub->rc, pmin, pmax, sub->residual,
                             s->frame.blocksize, pred_order);
    return bits;
}
