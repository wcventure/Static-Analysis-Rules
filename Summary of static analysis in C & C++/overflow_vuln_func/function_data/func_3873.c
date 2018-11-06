    dst_##type = (type*)dst; \
    for(;n>0;n--) { \
        register type v = bytestream_get_##endian(&src); \
        *dst_##type++ = (v - offset) << shift; \
    } \
