static void mxf_write_package(AVFormatContext *s, enum MXFMetadataSetType type, const char *package_name)
{
    MXFContext *mxf = s->priv_data;
    AVIOContext *pb = s->pb;
    int i, track_count = s->nb_streams+1;
    int name_size = mxf_utf16_local_tag_length(package_name);
    int user_comment_count = 0;

    if (type == MaterialPackage) {
        if (mxf->store_user_comments)
            user_comment_count = mxf_write_user_comments(s, s->metadata);
        mxf_write_metadata_key(pb, 0x013600);
        PRINT_KEY(s, "Material Package key", pb->buf_ptr - 16);
        klv_encode_ber_length(pb, 92 + name_size + (16*track_count) + (16*user_comment_count) + 12*mxf->store_user_comments);
    } else {
        mxf_write_metadata_key(pb, 0x013700);
        PRINT_KEY(s, "Source Package key", pb->buf_ptr - 16);
        klv_encode_ber_length(pb, 112 + name_size + (16*track_count) + 12*mxf->store_user_comments); // 20 bytes length for descriptor reference
    }

    // write uid
    mxf_write_local_tag(pb, 16, 0x3C0A);
    mxf_write_uuid(pb, type, 0);
    av_log(s,AV_LOG_DEBUG, "package type:%d\n", type);
    PRINT_KEY(s, "package uid", pb->buf_ptr - 16);

    // write package umid
    mxf_write_local_tag(pb, 32, 0x4401);
    mxf_write_umid(s, type == SourcePackage);
    PRINT_KEY(s, "package umid second part", pb->buf_ptr - 16);

    // package name
    if (name_size)
        mxf_write_local_tag_utf16(pb, 0x4402, package_name);

    // package creation date
    mxf_write_local_tag(pb, 8, 0x4405);
    avio_wb64(pb, mxf->timestamp);

    // package modified date
    mxf_write_local_tag(pb, 8, 0x4404);
    avio_wb64(pb, mxf->timestamp);

    // write track refs
    mxf_write_local_tag(pb, track_count*16 + 8, 0x4403);
    mxf_write_refs_count(pb, track_count);
    mxf_write_uuid(pb, type == MaterialPackage ? Track :
                   Track + TypeBottom, -1); // timecode track
    for (i = 0; i < s->nb_streams; i++)
        mxf_write_uuid(pb, type == MaterialPackage ? Track : Track + TypeBottom, i);

    // write user comment refs
    if (mxf->store_user_comments) {
        mxf_write_local_tag(pb, user_comment_count*16 + 8, 0x4406);
        mxf_write_refs_count(pb, user_comment_count);
        for (i = 0; i < user_comment_count; i++)
            mxf_write_uuid(pb, TaggedValue, mxf->tagged_value_count - user_comment_count + i);
    }

    // write multiple descriptor reference
    if (type == SourcePackage) {
        mxf_write_local_tag(pb, 16, 0x4701);
        if (s->nb_streams > 1) {
            mxf_write_uuid(pb, MultipleDescriptor, 0);
            mxf_write_multi_descriptor(s);
        } else
            mxf_write_uuid(pb, SubDescriptor, 0);
    }

    // write timecode track
    mxf_write_track(s, mxf->timecode_track, type);
    mxf_write_sequence(s, mxf->timecode_track, type);
    mxf_write_timecode_component(s, mxf->timecode_track, type);

    for (i = 0; i < s->nb_streams; i++) {
        AVStream *st = s->streams[i];
        mxf_write_track(s, st, type);
        mxf_write_sequence(s, st, type);
        mxf_write_structural_component(s, st, type);

        if (type == SourcePackage) {
            MXFStreamContext *sc = st->priv_data;
            mxf_essence_container_uls[sc->index].write_desc(s, st);
        }
    }
}
