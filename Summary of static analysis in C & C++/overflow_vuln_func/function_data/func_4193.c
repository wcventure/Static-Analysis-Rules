static int draw_slice(AVFilterLink *link, int y, int h, int slice_dir)
{
    SliceContext *slice = link->dst->priv;
    int y2, ret = 0;

    if (slice_dir == 1) {
        for (y2 = y; y2 + slice->h <= y + h; y2 += slice->h) {
            ret = ff_draw_slice(link->dst->outputs[0], y2, slice->h, slice_dir);
            if (ret < 0)
                return ret;
        }

        if (y2 < y + h)
            return ff_draw_slice(link->dst->outputs[0], y2, y + h - y2, slice_dir);
    } else if (slice_dir == -1) {
        for (y2 = y + h; y2 - slice->h >= y; y2 -= slice->h) {
            ret = ff_draw_slice(link->dst->outputs[0], y2 - slice->h, slice->h, slice_dir);
            if (ret < 0)
                return ret;
        }

        if (y2 > y)
            return ff_draw_slice(link->dst->outputs[0], y, y2 - y, slice_dir);
    }
    return 0;
}
