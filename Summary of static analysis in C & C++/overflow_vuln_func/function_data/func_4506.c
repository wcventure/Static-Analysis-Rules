static int mxf_read_index_table_segment(void *arg, AVIOContext *pb, int tag, int size, UID uid)
{
    switch(tag) {
    case 0x3F05: av_dlog(NULL, "EditUnitByteCount %d\n", avio_rb32(pb)); break;
    case 0x3F06: av_dlog(NULL, "IndexSID %d\n", avio_rb32(pb)); break;
    case 0x3F07: av_dlog(NULL, "BodySID %d\n", avio_rb32(pb)); break;
    case 0x3F0B: av_dlog(NULL, "IndexEditRate %d/%d\n", avio_rb32(pb), avio_rb32(pb)); break;
    case 0x3F0C: av_dlog(NULL, "IndexStartPosition %"PRIu64"\n", avio_rb64(pb)); break;
    case 0x3F0D: av_dlog(NULL, "IndexDuration %"PRIu64"\n", avio_rb64(pb)); break;
    }
    return 0;
}
