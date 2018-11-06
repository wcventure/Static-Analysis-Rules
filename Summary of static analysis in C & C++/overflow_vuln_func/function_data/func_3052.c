static int huf_decode(const uint64_t *hcode, const HufDec *hdecod,
                      GetByteContext *gb, int nbits,
                      int rlc, int no, uint16_t *out)
{
    uint64_t c        = 0;
    uint16_t *outb    = out;
    uint16_t *oe      = out + no;
    const uint8_t *ie = gb->buffer + (nbits + 7) / 8; // input byte size
    uint8_t cs, s;
    int i, lc = 0;

    while (gb->buffer < ie) {
        get_char(c, lc, gb);

        while (lc >= HUF_DECBITS) {
            const HufDec pl = hdecod[(c >> (lc - HUF_DECBITS)) & HUF_DECMASK];

            if (pl.len) {
                lc -= pl.len;
                get_code(pl.lit, rlc, c, lc, gb, out, oe);
            } else {
                int j;

                if (!pl.p)
                    return AVERROR_INVALIDDATA;

                for (j = 0; j < pl.lit; j++) {
                    int l = hcode[pl.p[j]] & 63;

                    while (lc < l && bytestream2_get_bytes_left(gb) > 0)
                        get_char(c, lc, gb);

                    if (lc >= l) {
                        if ((hcode[pl.p[j]] >> 6) ==
                            ((c >> (lc - l)) & ((1LL << l) - 1))) {
                            lc -= l;
                            get_code(pl.p[j], rlc, c, lc, gb, out, oe);
                            break;
                        }
                    }
                }

                if (j == pl.lit)
                    return AVERROR_INVALIDDATA;
            }
        }
    }

    i   = (8 - nbits) & 7;
    c >>= i;
    lc -= i;

    while (lc > 0) {
        const HufDec pl = hdecod[(c << (HUF_DECBITS - lc)) & HUF_DECMASK];

        if (pl.len) {
            lc -= pl.len;
            get_code(pl.lit, rlc, c, lc, gb, out, oe);
        } else {
            return AVERROR_INVALIDDATA;
        }
    }

    if (out - outb != no)
        return AVERROR_INVALIDDATA;
    return 0;
}
