static void mxf_write_multi_descriptor(AVFormatContext *s)
{
    MXFContext *mxf = s->priv_data;
    AVIOContext *pb = s->pb;
    const uint8_t *ul;
    int i;

    mxf_write_metadata_key(pb, 0x014400);
    PRINT_KEY(s, "multiple descriptor key", pb->buf_ptr - 16);
    klv_encode_ber_length(pb, 64 + 16 * s->nb_streams);

    mxf_write_local_tag(pb, 16, 0x3C0A);
    mxf_write_uuid(pb, MultipleDescriptor, 0);
    PRINT_KEY(s, "multi_desc uid", pb->buf_ptr - 16);

    // write sample rate
    mxf_write_local_tag(pb, 8, 0x3001);
    avio_wb32(pb, mxf->time_base.den);
    avio_wb32(pb, mxf->time_base.num);

    // write essence container ul
    mxf_write_local_tag(pb, 16, 0x3004);
    if (mxf->essence_container_count > 1)
        ul = multiple_desc_ul;
    else {
        MXFStreamContext *sc = s->streams[0]->priv_data;
        ul = mxf_essence_container_uls[sc->index].container_ul;
    }
    avio_write(pb, ul, 16);

    // write sub descriptor refs
    mxf_write_local_tag(pb, s->nb_streams * 16 + 8, 0x3F01);
    mxf_write_refs_count(pb, s->nb_streams);
    for (i = 0; i < s->nb_streams; i++)
        mxf_write_uuid(pb, SubDescriptor, i);
}
