int ff_amf_tag_size(const uint8_t *data, const uint8_t *data_end)
{
    const uint8_t *base = data;

    if (data >= data_end)
        return -1;
    switch (*data++) {
    case AMF_DATA_TYPE_NUMBER:      return 9;
    case AMF_DATA_TYPE_BOOL:        return 2;
    case AMF_DATA_TYPE_STRING:      return 3 + AV_RB16(data);
    case AMF_DATA_TYPE_LONG_STRING: return 5 + AV_RB32(data);
    case AMF_DATA_TYPE_NULL:        return 1;
    case AMF_DATA_TYPE_ARRAY:
        data += 4;
    case AMF_DATA_TYPE_OBJECT:
        for (;;) {
            int size = bytestream_get_be16(&data);
            int t;
            if (!size) {
                data++;
                break;
            }
            if (data + size >= data_end || data + size < data)
                return -1;
            data += size;
            t = ff_amf_tag_size(data, data_end);
            if (t < 0 || data + t >= data_end)
                return -1;
            data += t;
        }
        return data - base;
    case AMF_DATA_TYPE_OBJECT_END:  return 1;
    default:                        return -1;
    }
}
