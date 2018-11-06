int xbzrle_encode_buffer(uint8_t *old_buf, uint8_t *new_buf, int slen,
                         uint8_t *dst, int dlen)
{
    uint32_t zrun_len = 0, nzrun_len = 0;
    int d = 0, i = 0;
    long res, xor;
    uint8_t *nzrun_start = NULL;

    g_assert(!(((uintptr_t)old_buf | (uintptr_t)new_buf | slen) %
               sizeof(long)));

    while (i < slen) {
        /
        if (d + 2 > dlen) {
            return -1;
        }

        /
        res = (slen - i) % sizeof(long);
        while (res && old_buf[i] == new_buf[i]) {
            zrun_len++;
            i++;
            res--;
        }

        /
        if (!res) {
            while (i < slen &&
                   (*(long *)(old_buf + i)) == (*(long *)(new_buf + i))) {
                i += sizeof(long);
                zrun_len += sizeof(long);
            }

            /
            while (i < slen && old_buf[i] == new_buf[i]) {
                zrun_len++;
                i++;
            }
        }

        /
        if (zrun_len == slen) {
            return 0;
        }

        /
        if (i == slen) {
            return d;
        }

        d += uleb128_encode_small(dst + d, zrun_len);

        zrun_len = 0;
        nzrun_start = new_buf + i;

        /
        if (d + 2 > dlen) {
            return -1;
        }
        /
        res = (slen - i) % sizeof(long);
        while (res && old_buf[i] != new_buf[i]) {
            i++;
            nzrun_len++;
            res--;
        }

        /
        if (!res) {
            /
            long mask = 0x0101010101010101ULL;
            while (i < slen) {
                xor = *(long *)(old_buf + i) ^ *(long *)(new_buf + i);
                if ((xor - mask) & ~xor & (mask << 7)) {
                    /
                    while (old_buf[i] != new_buf[i]) {
                        nzrun_len++;
                        i++;
                    }
                    break;
                } else {
                    i += sizeof(long);
                    nzrun_len += sizeof(long);
                }
            }
        }

        d += uleb128_encode_small(dst + d, nzrun_len);
        /
        if (d + nzrun_len > dlen) {
            return -1;
        }
        memcpy(dst + d, nzrun_start, nzrun_len);
        d += nzrun_len;
        nzrun_len = 0;
    }

    return d;
}
