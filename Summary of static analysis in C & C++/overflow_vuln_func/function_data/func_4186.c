static void ff_amf_tag_contents(void *ctx, const uint8_t *data, const uint8_t *data_end)
{
    int size;
    char buf[1024];

    if (data >= data_end)
        return;
    switch (*data++) {
    case AMF_DATA_TYPE_NUMBER:
        av_log(ctx, AV_LOG_DEBUG, " number %g\n", av_int2double(AV_RB64(data)));
        return;
    case AMF_DATA_TYPE_BOOL:
        av_log(ctx, AV_LOG_DEBUG, " bool %d\n", *data);
        return;
    case AMF_DATA_TYPE_STRING:
    case AMF_DATA_TYPE_LONG_STRING:
        if (data[-1] == AMF_DATA_TYPE_STRING) {
            size = bytestream_get_be16(&data);
        } else {
            size = bytestream_get_be32(&data);
        }
        size = FFMIN(size, 1023);
        memcpy(buf, data, size);
        buf[size] = 0;
        av_log(ctx, AV_LOG_DEBUG, " string '%s'\n", buf);
        return;
    case AMF_DATA_TYPE_NULL:
        av_log(ctx, AV_LOG_DEBUG, " NULL\n");
        return;
    case AMF_DATA_TYPE_ARRAY:
        data += 4;
    case AMF_DATA_TYPE_OBJECT:
        av_log(ctx, AV_LOG_DEBUG, " {\n");
        for (;;) {
            int size = bytestream_get_be16(&data);
            int t;
            memcpy(buf, data, size);
            buf[size] = 0;
            if (!size) {
                av_log(ctx, AV_LOG_DEBUG, " }\n");
                data++;
                break;
            }
            if (size < 0 || size >= data_end - data)
                return;
            data += size;
            av_log(ctx, AV_LOG_DEBUG, "  %s: ", buf);
            ff_amf_tag_contents(ctx, data, data_end);
            t = ff_amf_tag_size(data, data_end);
            if (t < 0 || t >= data_end - data)
                return;
            data += t;
        }
        return;
    case AMF_DATA_TYPE_OBJECT_END:
        av_log(ctx, AV_LOG_DEBUG, " }\n");
        return;
    default:
        return;
    }
}
