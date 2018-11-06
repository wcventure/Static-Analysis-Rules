static inline int mxf_read_utf16_string(AVIOContext *pb, int size, char** str, int be)
{
    int ret;
    size_t buf_size;

    if (size < 0)
        return AVERROR(EINVAL);

    buf_size = size + size / 2 + 1;
    *str = av_malloc(buf_size);
    if (!*str)
        return AVERROR(ENOMEM);

    if (be)
        ret = avio_get_str16be(pb, size, *str, buf_size);
    else
        ret = avio_get_str16le(pb, size, *str, buf_size);

    if (ret < 0) {
        av_freep(str);
        return ret;
    }

    return ret;
}
