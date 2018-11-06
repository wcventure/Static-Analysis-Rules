static const uint8_t *read_huffman_tables(FourXContext *f,
                                          const uint8_t * const buf, int buf_size)
{
    int frequency[512] = { 0 };
    uint8_t flag[512];
    int up[512];
    uint8_t len_tab[257];
    int bits_tab[257];
    int start, end;
    const uint8_t *ptr = buf;
    const uint8_t *ptr_end = buf + buf_size;
    int j;

    memset(up, -1, sizeof(up));

    start = *ptr++;
    end   = *ptr++;
    for (;;) {
        int i;

        if (start <= end && ptr_end - ptr < end - start + 1 + 1)
            return NULL;
        for (i = start; i <= end; i++)
            frequency[i] = *ptr++;
        start = *ptr++;
        if (start == 0)
            break;

        end = *ptr++;
    }
    frequency[256] = 1;

    while ((ptr - buf) & 3)
        ptr++; // 4byte align

    for (j = 257; j < 512; j++) {
        int min_freq[2] = { 256 * 256, 256 * 256 };
        int smallest[2] = { 0, 0 };
        int i;
        for (i = 0; i < j; i++) {
            if (frequency[i] == 0)
                continue;
            if (frequency[i] < min_freq[1]) {
                if (frequency[i] < min_freq[0]) {
                    min_freq[1] = min_freq[0];
                    smallest[1] = smallest[0];
                    min_freq[0] = frequency[i];
                    smallest[0] = i;
                } else {
                    min_freq[1] = frequency[i];
                    smallest[1] = i;
                }
            }
        }
        if (min_freq[1] == 256 * 256)
            break;

        frequency[j]           = min_freq[0] + min_freq[1];
        flag[smallest[0]]      = 0;
        flag[smallest[1]]      = 1;
        up[smallest[0]]        =
        up[smallest[1]]        = j;
        frequency[smallest[0]] = frequency[smallest[1]] = 0;
    }

    for (j = 0; j < 257; j++) {
        int node, len = 0, bits = 0;

        for (node = j; up[node] != -1; node = up[node]) {
            bits += flag[node] << len;
            len++;
            if (len > 31)
                // can this happen at all ?
                av_log(f->avctx, AV_LOG_ERROR,
                       "vlc length overflow\n");
        }

        bits_tab[j] = bits;
        len_tab[j]  = len;
    }

    if (init_vlc(&f->pre_vlc, ACDC_VLC_BITS, 257, len_tab, 1, 1,
                 bits_tab, 4, 4, 0))
        return NULL;

    return ptr;
}
