static int64_t mmsh_seek(URLContext *h, int64_t pos, int whence)
{
    MMSHContext *mmsh = h->priv_data;
    MMSContext *mms   = &mmsh->mms;

    if(pos == 0 && whence == SEEK_CUR)
        return mms->asf_header_read_size + mms->remaining_in_len + mmsh->chunk_seq * mms->asf_packet_len;
    return AVERROR(ENOSYS);
}
