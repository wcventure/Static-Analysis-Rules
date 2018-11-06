static int start_frame(AVFilterLink *link, AVFilterBufferRef *picref)
{
    SliceContext *slice = link->dst->priv;

    if (slice->use_random_h) {
        slice->lcg_state = slice->lcg_state * 1664525 + 1013904223;
        slice->h = 8 + (uint64_t)slice->lcg_state * 25 / UINT32_MAX;
    }

    /
    slice->h = FFMAX(8, slice->h & (-1 << slice->vshift));

    av_log(link->dst, AV_LOG_DEBUG, "h:%d\n", slice->h);
    link->cur_buf = NULL;

    return ff_start_frame(link->dst->outputs[0], picref);
}
