static int recover(WtvContext *wtv, uint64_t broken_pos)
{
    AVIOContext *pb = wtv->pb;
    int i;
    for (i = 0; i < wtv->nb_index_entries; i++) {
        if (wtv->index_entries[i].pos > broken_pos) {
            int ret = avio_seek(pb, wtv->index_entries[i].pos, SEEK_SET);
            if (ret < 0)
                return ret;
            wtv->pts = wtv->index_entries[i].timestamp;
            return 0;
         }
     }
     return AVERROR(EIO);
}
