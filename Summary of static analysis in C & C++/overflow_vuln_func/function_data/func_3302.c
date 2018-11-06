static int update_offset(RTMPContext *rt, int size)
{
    int old_flv_size;

    // generate packet header and put data into buffer for FLV demuxer
    if (rt->flv_off < rt->flv_size) {
        // There is old unread data in the buffer, thus append at the end
        old_flv_size  = rt->flv_size;
        rt->flv_size += size + 15;
    } else {
        // All data has been read, write the new data at the start of the buffer
        old_flv_size = 0;
        rt->flv_size = size + 15;
        rt->flv_off  = 0;
    }

    return old_flv_size;
}
