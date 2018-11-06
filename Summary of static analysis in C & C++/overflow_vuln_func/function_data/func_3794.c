    if(buf + needed > buf_end){ \
        av_log(avctx, AV_LOG_ERROR, "Problem: unexpected end of data while reading " where "\n"); \
        return -1; \
    } \
