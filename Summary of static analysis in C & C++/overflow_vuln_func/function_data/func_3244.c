static void read_tree(GetBitContext *gb, Tree *tree)
{
    uint8_t tmp1[16], tmp2[16], *in = tmp1, *out = tmp2;
    int i, t, len;

    tree->vlc_num = get_bits(gb, 4);
    if (!tree->vlc_num) {
        for (i = 0; i < 16; i++)
            tree->syms[i] = i;
        return;
    }
    if (get_bits1(gb)) {
        len = get_bits(gb, 3);
        memset(tmp1, 0, sizeof(tmp1));
        for (i = 0; i <= len; i++) {
            tree->syms[i] = get_bits(gb, 4);
            tmp1[tree->syms[i]] = 1;
        }
        for (i = 0; i < 16; i++)
            if (!tmp1[i])
                tree->syms[++len] = i;
    } else {
        len = get_bits(gb, 2);
        for (i = 0; i < 16; i++)
            in[i] = i;
        for (i = 0; i <= len; i++) {
            int size = 1 << i;
            for (t = 0; t < 16; t += size << 1)
                merge(gb, out + t, in + t, size);
            FFSWAP(uint8_t*, in, out);
        }
        memcpy(tree->syms, in, 16);
    }
}
