static int subframe_count_exact(FlacEncodeContext *s, FlacSubframe *sub,
                                int pred_order)
{
    int p, porder, psize;
    int i, part_end;
    int count = 0;

    /
    count += 8;

    /
    if (sub->type == FLAC_SUBFRAME_CONSTANT) {
        count += sub->obits;
    } else if (sub->type == FLAC_SUBFRAME_VERBATIM) {
        count += s->frame.blocksize * sub->obits;
    } else {
        /
        count += pred_order * sub->obits;

        /
        if (sub->type == FLAC_SUBFRAME_LPC)
            count += 4 + 5 + pred_order * s->options.lpc_coeff_precision;

        /
        count += 2;

        /
        porder = sub->rc.porder;
        psize  = s->frame.blocksize >> porder;
        count += 4;

        /
        i        = pred_order;
        part_end = psize;
        for (p = 0; p < 1 << porder; p++) {
            int k = sub->rc.params[p];
            count += 4;
            count += rice_count_exact(&sub->residual[i], part_end - i, k);
            i = part_end;
            part_end = FFMIN(s->frame.blocksize, part_end + psize);
        }
    }

    return count;
}
