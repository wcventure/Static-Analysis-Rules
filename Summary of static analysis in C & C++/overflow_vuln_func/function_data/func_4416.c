static int request_frame(AVFilterLink *outlink)
{
    TestSourceContext *test = outlink->src->priv;
    AVFilterBufferRef *picref;

    if (test->max_pts >= 0 && test->pts > test->max_pts)
        return AVERROR_EOF;
    picref = avfilter_get_video_buffer(outlink, AV_PERM_WRITE,
                                       test->w, test->h);
    picref->pts = test->pts++;
    picref->pos = -1;
    picref->video->key_frame = 1;
    picref->video->interlaced = 0;
    picref->video->pict_type = AV_PICTURE_TYPE_I;
    picref->video->sample_aspect_ratio = test->sar;
    test->fill_picture_fn(outlink->src, picref);
    test->nb_frame++;

    avfilter_start_frame(outlink, avfilter_ref_buffer(picref, ~0));
    avfilter_draw_slice(outlink, 0, picref->video->h, 1);
    avfilter_end_frame(outlink);
    avfilter_unref_buffer(picref);

    return 0;
}
