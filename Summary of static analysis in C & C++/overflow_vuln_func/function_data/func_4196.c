static void mxf_write_system_item(AVFormatContext *s)
{
    MXFContext *mxf = s->priv_data;
    AVIOContext *pb = s->pb;
    unsigned frame;
    uint32_t time_code;

    frame = mxf->last_indexed_edit_unit + mxf->edit_units_count;

    // write system metadata pack
    avio_write(pb, system_metadata_pack_key, 16);
    klv_encode_ber4_length(pb, 57);
    avio_w8(pb, 0x5c); // UL, user date/time stamp, picture and sound item present
    avio_w8(pb, 0x04); // content package rate
    avio_w8(pb, 0x00); // content package type
    avio_wb16(pb, 0x00); // channel handle
    avio_wb16(pb, mxf->tc.start + frame); // continuity count
    if (mxf->essence_container_count > 1)
        avio_write(pb, multiple_desc_ul, 16);
    else {
        MXFStreamContext *sc = s->streams[0]->priv_data;
        avio_write(pb, mxf_essence_container_uls[sc->index].container_ul, 16);
    }
    avio_w8(pb, 0);
    avio_wb64(pb, 0);
    avio_wb64(pb, 0); // creation date/time stamp

    avio_w8(pb, 0x81); // SMPTE 12M time code
    time_code = av_timecode_get_smpte_from_framenum(&mxf->tc, frame);
    avio_wb32(pb, time_code);
    avio_wb32(pb, 0); // binary group data
    avio_wb64(pb, 0);

    // write system metadata package set
    avio_write(pb, system_metadata_package_set_key, 16);
    klv_encode_ber4_length(pb, 35);
    avio_w8(pb, 0x83); // UMID
    avio_wb16(pb, 0x20);
    mxf_write_umid(s, 1);
}
