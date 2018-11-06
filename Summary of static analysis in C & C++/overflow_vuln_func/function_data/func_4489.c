void ff_hevc_set_qPy(HEVCContext *s, int xC, int yC,
                     int xBase, int yBase, int log2_cb_size)
{
    int qp_y = get_qPy_pred(s, xC, yC, xBase, yBase, log2_cb_size);

    if (s->HEVClc->tu.cu_qp_delta != 0) {
        int off = s->sps->qp_bd_offset;
        s->HEVClc->qp_y = ((qp_y + s->HEVClc->tu.cu_qp_delta + 52 + 2 * off) %
                          (52 + off)) - off;
    } else
        s->HEVClc->qp_y = qp_y;
}
