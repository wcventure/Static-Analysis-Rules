static int hls_coding_quadtree(HEVCContext *s, int x0, int y0,
                               int log2_cb_size, int cb_depth)
{
    HEVCLocalContext *lc = s->HEVClc;
    const int cb_size    = 1 << log2_cb_size;
    int ret;

    lc->ct.depth = cb_depth;
    if (x0 + cb_size <= s->sps->width  &&
        y0 + cb_size <= s->sps->height &&
        log2_cb_size > s->sps->log2_min_cb_size) {
        SAMPLE(s->split_cu_flag, x0, y0) =
            ff_hevc_split_coding_unit_flag_decode(s, cb_depth, x0, y0);
    } else {
        SAMPLE(s->split_cu_flag, x0, y0) =
            (log2_cb_size > s->sps->log2_min_cb_size);
    }
    if (s->pps->cu_qp_delta_enabled_flag &&
        log2_cb_size >= s->sps->log2_ctb_size - s->pps->diff_cu_qp_delta_depth) {
        lc->tu.is_cu_qp_delta_coded = 0;
        lc->tu.cu_qp_delta          = 0;
    }

    if (SAMPLE(s->split_cu_flag, x0, y0)) {
        const int cb_size_split = cb_size >> 1;
        const int x1 = x0 + cb_size_split;
        const int y1 = y0 + cb_size_split;

        int more_data = 0;

        more_data = hls_coding_quadtree(s, x0, y0, log2_cb_size - 1, cb_depth + 1);
        if (more_data < 0)
            return more_data;

        if (more_data && x1 < s->sps->width) {
            more_data = hls_coding_quadtree(s, x1, y0, log2_cb_size - 1, cb_depth + 1);
            if (more_data < 0)
                return more_data;
        }
        if (more_data && y1 < s->sps->height) {
            more_data = hls_coding_quadtree(s, x0, y1, log2_cb_size - 1, cb_depth + 1);
            if (more_data < 0)
                return more_data;
        }
        if (more_data && x1 < s->sps->width &&
            y1 < s->sps->height) {
            return hls_coding_quadtree(s, x1, y1, log2_cb_size - 1, cb_depth + 1);
        }
        if (more_data)
            return ((x1 + cb_size_split) < s->sps->width ||
                    (y1 + cb_size_split) < s->sps->height);
        else
            return 0;
    } else {
        ret = hls_coding_unit(s, x0, y0, log2_cb_size);
        if (ret < 0)
            return ret;
        if ((!((x0 + cb_size) %
               (1 << (s->sps->log2_ctb_size))) ||
             (x0 + cb_size >= s->sps->width)) &&
            (!((y0 + cb_size) %
               (1 << (s->sps->log2_ctb_size))) ||
             (y0 + cb_size >= s->sps->height))) {
            int end_of_slice_flag = ff_hevc_end_of_slice_flag_decode(s);
            return !end_of_slice_flag;
        } else {
            return 1;
        }
    }

    return 0;
}
