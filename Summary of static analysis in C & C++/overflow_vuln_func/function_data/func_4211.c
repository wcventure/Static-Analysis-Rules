static int vp8_alloc_frame(VP8Context *s, AVFrame *f)
{
    int ret;
    if ((ret = ff_thread_get_buffer(s->avctx, f)) < 0)
        return ret;
    if (!s->maps_are_invalid && s->num_maps_to_be_freed) {
        f->ref_index[0] = s->segmentation_maps[--s->num_maps_to_be_freed];
    } else if (!(f->ref_index[0] = av_mallocz(s->mb_width * s->mb_height))) {
        ff_thread_release_buffer(s->avctx, f);
        return AVERROR(ENOMEM);
    }
    return 0;
}
