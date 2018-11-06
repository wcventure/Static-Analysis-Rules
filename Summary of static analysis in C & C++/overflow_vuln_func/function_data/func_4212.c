static void vp8_release_frame(VP8Context *s, AVFrame *f, int is_close)
{
    if (!is_close) {
        if (f->ref_index[0]) {
            assert(s->num_maps_to_be_freed < FF_ARRAY_ELEMS(s->segmentation_maps));
            s->segmentation_maps[s->num_maps_to_be_freed++] = f->ref_index[0];
            f->ref_index[0] = NULL;
        }
    } else {
        av_freep(&f->ref_index[0]);
    }
    ff_thread_release_buffer(s->avctx, f);
}
