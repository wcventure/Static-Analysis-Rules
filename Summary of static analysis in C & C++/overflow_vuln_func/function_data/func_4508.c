static int mxf_read_close(AVFormatContext *s)
{
    MXFContext *mxf = s->priv_data;
    int i;

    av_freep(&mxf->packages_refs);

    for (i = 0; i < s->nb_streams; i++)
        s->streams[i]->priv_data = NULL;

    for (i = 0; i < mxf->metadata_sets_count; i++) {
        switch (mxf->metadata_sets[i]->type) {
        case MultipleDescriptor:
            av_freep(&((MXFDescriptor *)mxf->metadata_sets[i])->sub_descriptors_refs);
            break;
        case Sequence:
            av_freep(&((MXFSequence *)mxf->metadata_sets[i])->structural_components_refs);
            break;
        case SourcePackage:
        case MaterialPackage:
            av_freep(&((MXFPackage *)mxf->metadata_sets[i])->tracks_refs);
            break;
        default:
            break;
        }
        av_freep(&mxf->metadata_sets[i]);
    }
    av_freep(&mxf->partitions);
    av_freep(&mxf->metadata_sets);
    av_freep(&mxf->aesc);
    av_freep(&mxf->local_tags);
    return 0;
}
