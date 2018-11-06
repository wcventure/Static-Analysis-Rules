{                                                                             \
        if (po == rlc) {                                                      \
            if (lc < 8)                                                       \
                get_char(c, lc, gb);                                          \
            lc -= 8;                                                          \
                                                                              \
            cs = c >> lc;                                                     \
                                                                              \
            if (out + cs > oe)                                                \
                return AVERROR_INVALIDDATA;                                   \
                                                                              \
            s = out[-1];                                                      \
                                                                              \
            while (cs-- > 0)                                                  \
                *out++ = s;                                                   \
        } else if (out < oe) {                                                \
            *out++ = po;                                                      \
        } else {                                                              \
            return AVERROR_INVALIDDATA;                                       \
        }                                                                     \
}
