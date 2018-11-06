int ff_amf_tag_size(const uint8_t *data, const uint8_t *data_end)
{
    const uint8_t *base = data;
    AMFDataType type;
    unsigned nb   = -1;
    int parse_key = 1;

    if (data >= data_end)
        return -1;
    switch ((type = *data++)) {
    case AMF_DATA_TYPE_NUMBER:      return 9;
    case AMF_DATA_TYPE_BOOL:        return 2;
    case AMF_DATA_TYPE_STRING:      return 3 + AV_RB16(data);
    case AMF_DATA_TYPE_LONG_STRING: return 5 + AV_RB32(data);
    case AMF_DATA_TYPE_NULL:        return 1;
    case AMF_DATA_TYPE_DATE:        return 11;
    case AMF_DATA_TYPE_ARRAY:
        parse_key = 0;
    case AMF_DATA_TYPE_MIXEDARRAY:
        nb = bytestream_get_be32(&data);
    case AMF_DATA_TYPE_OBJECT:
        while (nb-- > 0 || type != AMF_DATA_TYPE_ARRAY) {
            int t;
            if (parse_key) {
                int size = bytestream_get_be16(&data);
                if (!size) {
                    data++;
                    break;
                }
                if (size < 0 || size >= data_end - data)
                    return -1;
                data += size;
            }
            t = ff_amf_tag_size(data, data_end);
            if (t < 0 || t >= data_end - data)
                return -1;
            data += t;
        }
        return data - base;
    case AMF_DATA_TYPE_OBJECT_END:  return 1;
    default:                        return -1;
    }
}
