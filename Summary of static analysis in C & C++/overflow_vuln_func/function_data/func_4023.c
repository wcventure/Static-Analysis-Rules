static void mxf_write_random_index_pack(AVFormatContext *s)
{
    MXFContext *mxf = s->priv_data;
    AVIOContext *pb = s->pb;
    uint64_t pos = avio_tell(pb);
    int i;

    avio_write(pb, random_index_pack_key, 16);
    klv_encode_ber_length(pb, 28 + 12*mxf->body_partitions_count);

    if (mxf->edit_unit_byte_count)
        avio_wb32(pb, 1); // BodySID of header partition
    else
        avio_wb32(pb, 0);
    avio_wb64(pb, 0); // offset of header partition

    for (i = 0; i < mxf->body_partitions_count; i++) {
        avio_wb32(pb, 1); // BodySID
        avio_wb64(pb, mxf->body_partition_offset[i]);
    }

    avio_wb32(pb, 0); // BodySID of footer partition
    avio_wb64(pb, mxf->footer_partition_offset);

    avio_wb32(pb, avio_tell(pb) - pos + 4);
}
