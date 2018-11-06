static int packet_alloc(AVBufferRef **buf, int size)
{
    int ret;
    if ((unsigned)size >= (unsigned)size + AV_INPUT_BUFFER_PADDING_SIZE)
        return AVERROR(EINVAL);

    ret = av_buffer_realloc(buf, size + AV_INPUT_BUFFER_PADDING_SIZE);
    if (ret < 0)
        return ret;

    memset((*buf)->data + size, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    return 0;
}
