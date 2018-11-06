static int mxf_write_partition(AVFormatContext *s, int bodysid,
                                int indexsid,
                                const uint8_t *key, int write_metadata)
{
    MXFContext *mxf = s->priv_data;
    AVIOContext *pb = s->pb;
    int64_t header_byte_count_offset;
    unsigned index_byte_count = 0;
    uint64_t partition_offset = avio_tell(pb);
    int err;

    if (!mxf->edit_unit_byte_count && mxf->edit_units_count)
        index_byte_count = 85 + 12+(s->nb_streams+1)*6 +
            12+mxf->edit_units_count*(11+mxf->slice_count*4);
    else if (mxf->edit_unit_byte_count && indexsid)
        index_byte_count = 80;

    if (index_byte_count) {
        // add encoded ber length
        index_byte_count += 16 + klv_ber_length(index_byte_count);
        index_byte_count += klv_fill_size(index_byte_count);
    }

    if (!memcmp(key, body_partition_key, 16)) {
        if ((err = av_reallocp_array(&mxf->body_partition_offset, mxf->body_partitions_count + 1,
                                     sizeof(*mxf->body_partition_offset))) < 0) {
            mxf->body_partitions_count = 0;
            return err;
        }
        mxf->body_partition_offset[mxf->body_partitions_count++] = partition_offset;
    }

    // write klv
    avio_write(pb, key, 16);
    klv_encode_ber_length(pb, 88 + 16 * mxf->essence_container_count);

    // write partition value
    avio_wb16(pb, 1); // majorVersion
    avio_wb16(pb, 2); // minorVersion
    avio_wb32(pb, KAG_SIZE); // KAGSize

    avio_wb64(pb, partition_offset); // ThisPartition

    if (!memcmp(key, body_partition_key, 16) && mxf->body_partitions_count > 1)
        avio_wb64(pb, mxf->body_partition_offset[mxf->body_partitions_count-2]); // PreviousPartition
    else if (!memcmp(key, footer_partition_key, 16) && mxf->body_partitions_count)
        avio_wb64(pb, mxf->body_partition_offset[mxf->body_partitions_count-1]); // PreviousPartition
    else
        avio_wb64(pb, 0);

    avio_wb64(pb, mxf->footer_partition_offset); // footerPartition

    // set offset
    header_byte_count_offset = avio_tell(pb);
    avio_wb64(pb, 0); // headerByteCount, update later

    // indexTable
    avio_wb64(pb, index_byte_count); // indexByteCount
    avio_wb32(pb, index_byte_count ? indexsid : 0); // indexSID

    // BodyOffset
    if (bodysid && mxf->edit_units_count && mxf->body_partitions_count) {
        avio_wb64(pb, mxf->body_offset);
    } else
        avio_wb64(pb, 0);

    avio_wb32(pb, bodysid); // bodySID

    // operational pattern
    avio_write(pb, op1a_ul, 16);

    // essence container
    mxf_write_essence_container_refs(s);

    if (write_metadata) {
        // mark the start of the headermetadata and calculate metadata size
        int64_t pos, start;
        unsigned header_byte_count;

        mxf_write_klv_fill(s);
        start = avio_tell(s->pb);
        mxf_write_primer_pack(s);
        mxf_write_header_metadata_sets(s);
        pos = avio_tell(s->pb);
        header_byte_count = pos - start + klv_fill_size(pos);

        // update header_byte_count
        avio_seek(pb, header_byte_count_offset, SEEK_SET);
        avio_wb64(pb, header_byte_count);
        avio_seek(pb, pos, SEEK_SET);
    }

    avio_flush(pb);

    return 0;
}
