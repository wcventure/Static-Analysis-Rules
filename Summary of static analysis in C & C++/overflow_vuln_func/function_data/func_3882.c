static int rm_read_header_old(AVFormatContext *s, AVFormatParameters *ap)
{
    RMContext *rm = s->priv_data;
    AVStream *st;

    rm->old_format = 1;
    st = av_new_stream(s, 0);
    if (!st)
        goto fail;
    rm_read_audio_stream_info(s, st, 1);
    return 0;
 fail:
    return -1;
}
