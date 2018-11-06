static int get_qPy_pred(HEVCContext *s, int xC, int yC,
                        int xBase, int yBase, int log2_cb_size)
{
    HEVCLocalContext *lc     = s->HEVClc;
    int ctb_size_mask        = (1 << s->sps->log2_ctb_size) - 1;
    int MinCuQpDeltaSizeMask = (1 << (s->sps->log2_ctb_size -
                                      s->pps->diff_cu_qp_delta_depth)) - 1;
    int xQgBase              = xBase - (xBase & MinCuQpDeltaSizeMask);
    int yQgBase              = yBase - (yBase & MinCuQpDeltaSizeMask);
    int min_cb_width         = s->sps->min_cb_width;
    int min_cb_height        = s->sps->min_cb_height;
    int x_cb                 = xQgBase >> s->sps->log2_min_cb_size;
    int y_cb                 = yQgBase >> s->sps->log2_min_cb_size;
    int availableA           = (xBase   & ctb_size_mask) &&
                               (xQgBase & ctb_size_mask);
    int availableB           = (yBase   & ctb_size_mask) &&
                               (yQgBase & ctb_size_mask);
    int qPy_pred, qPy_a, qPy_b;

    // qPy_pred
    if (lc->first_qp_group || (!xQgBase && !yQgBase)) {
        lc->first_qp_group = !lc->tu.is_cu_qp_delta_coded;
        qPy_pred = s->sh.slice_qp;
    } else {
        qPy_pred = lc->qp_y;
        if (log2_cb_size < s->sps->log2_ctb_size -
                           s->pps->diff_cu_qp_delta_depth) {
            static const int offsetX[8][8] = {
                { -1, 1, 3, 1, 7, 1, 3, 1 },
                {  0, 0, 0, 0, 0, 0, 0, 0 },
                {  1, 3, 1, 3, 1, 3, 1, 3 },
                {  2, 2, 2, 2, 2, 2, 2, 2 },
                {  3, 5, 7, 5, 3, 5, 7, 5 },
                {  4, 4, 4, 4, 4, 4, 4, 4 },
                {  5, 7, 5, 7, 5, 7, 5, 7 },
                {  6, 6, 6, 6, 6, 6, 6, 6 }
            };
            static const int offsetY[8][8] = {
                { 7, 0, 1, 2, 3, 4, 5, 6 },
                { 0, 1, 2, 3, 4, 5, 6, 7 },
                { 1, 0, 3, 2, 5, 4, 7, 6 },
                { 0, 1, 2, 3, 4, 5, 6, 7 },
                { 3, 0, 1, 2, 7, 4, 5, 6 },
                { 0, 1, 2, 3, 4, 5, 6, 7 },
                { 1, 0, 3, 2, 5, 4, 7, 6 },
                { 0, 1, 2, 3, 4, 5, 6, 7 }
            };
            int xC0b = (xC - (xC & ctb_size_mask)) >> s->sps->log2_min_cb_size;
            int yC0b = (yC - (yC & ctb_size_mask)) >> s->sps->log2_min_cb_size;
            int idxX = (xQgBase  & ctb_size_mask)  >> s->sps->log2_min_cb_size;
            int idxY = (yQgBase  & ctb_size_mask)  >> s->sps->log2_min_cb_size;
            int idx_mask = ctb_size_mask >> s->sps->log2_min_cb_size;
            int x, y;

            x = FFMIN(xC0b +  offsetX[idxX][idxY],             min_cb_width  - 1);
            y = FFMIN(yC0b + (offsetY[idxX][idxY] & idx_mask), min_cb_height - 1);

            if (xC0b == (lc->start_of_tiles_x >> s->sps->log2_min_cb_size) &&
                offsetX[idxX][idxY] == -1) {
                x = (lc->end_of_tiles_x >> s->sps->log2_min_cb_size) - 1;
                y = yC0b - 1;
            }
            qPy_pred = s->qp_y_tab[y * min_cb_width + x];
        }
    }

    // qPy_a
    if (availableA == 0)
        qPy_a = qPy_pred;
    else
        qPy_a = s->qp_y_tab[(x_cb - 1) + y_cb * min_cb_width];

    // qPy_b
    if (availableB == 0)
        qPy_b = qPy_pred;
    else
        qPy_b = s->qp_y_tab[x_cb + (y_cb - 1) * min_cb_width];

    av_assert2(qPy_a >= -s->sps->qp_bd_offset && qPy_a < 52);
    av_assert2(qPy_b >= -s->sps->qp_bd_offset && qPy_b < 52);

    return (qPy_a + qPy_b + 1) >> 1;
}
