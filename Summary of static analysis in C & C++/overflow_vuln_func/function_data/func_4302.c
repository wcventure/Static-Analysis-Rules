static int mpegts_audio_write(void *opaque, uint8_t *buf, int size)
{
    MpegTSWriteStream *ts_st = (MpegTSWriteStream *)opaque;
    if (ts_st->adata_pos + size > ts_st->adata_size)
        return AVERROR(EIO);

    memcpy(ts_st->adata + ts_st->adata_pos, buf, size);
    ts_st->adata_pos += size;

    return 0;
}
