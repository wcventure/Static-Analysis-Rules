static int pic_arrays_init(HEVCContext *s, const HEVCSPS *sps)
{
    int log2_min_cb_size = sps->log2_min_cb_size;
    int width            = sps->width;
    int height           = sps->height;
    int pic_size_in_ctb  = ((width  >> log2_min_cb_size) + 1) *
                           ((height >> log2_min_cb_size) + 1);
    int ctb_count        = sps->ctb_width * sps->ctb_height;
    int min_pu_size      = sps->min_pu_width * sps->min_pu_height;

    s->bs_width  = (width  >> 2) + 1;
    s->bs_height = (height >> 2) + 1;

    s->sao           = av_mallocz_array(ctb_count, sizeof(*s->sao));
    s->deblock       = av_mallocz_array(ctb_count, sizeof(*s->deblock));
    if (!s->sao || !s->deblock)
        goto fail;

    s->skip_flag    = av_malloc(sps->min_cb_height * sps->min_cb_width);
    s->tab_ct_depth = av_malloc_array(sps->min_cb_height, sps->min_cb_width);
    if (!s->skip_flag || !s->tab_ct_depth)
        goto fail;

    s->cbf_luma = av_malloc_array(sps->min_tb_width, sps->min_tb_height);
    s->tab_ipm  = av_mallocz(min_pu_size);
    s->is_pcm   = av_malloc((sps->min_pu_width + 1) * (sps->min_pu_height + 1));
    if (!s->tab_ipm || !s->cbf_luma || !s->is_pcm)
        goto fail;

    s->filter_slice_edges = av_malloc(ctb_count);
    s->tab_slice_address  = av_malloc_array(pic_size_in_ctb,
                                      sizeof(*s->tab_slice_address));
    s->qp_y_tab           = av_malloc_array(pic_size_in_ctb,
                                      sizeof(*s->qp_y_tab));
    if (!s->qp_y_tab || !s->filter_slice_edges || !s->tab_slice_address)
        goto fail;

    s->horizontal_bs = av_mallocz_array(s->bs_width, s->bs_height);
    s->vertical_bs   = av_mallocz_array(s->bs_width, s->bs_height);
    if (!s->horizontal_bs || !s->vertical_bs)
        goto fail;

    s->tab_mvf_pool = av_buffer_pool_init(min_pu_size * sizeof(MvField),
                                          av_buffer_allocz);
    s->rpl_tab_pool = av_buffer_pool_init(ctb_count * sizeof(RefPicListTab),
                                          av_buffer_allocz);
    if (!s->tab_mvf_pool || !s->rpl_tab_pool)
        goto fail;

    return 0;

fail:
    pic_arrays_free(s);
    return AVERROR(ENOMEM);
}
