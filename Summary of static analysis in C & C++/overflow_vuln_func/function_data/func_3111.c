static CodeBook unpack_codebook(GetBitContext* gb, unsigned depth,
                                 unsigned size)
{
    unsigned i, j;
    CodeBook cb = { 0 };

    if (!can_safely_read(gb, size * 34))
        return cb;

    if (size >= INT_MAX / sizeof(MacroBlock))
        return cb;
    cb.blocks = av_malloc(size ? size * sizeof(MacroBlock) : 1);
    if (!cb.blocks)
        return cb;

    cb.depth = depth;
    cb.size = size;
    for (i = 0; i < size; i++) {
        unsigned mask_bits = get_bits(gb, 4);
        unsigned color0 = get_bits(gb, 15);
        unsigned color1 = get_bits(gb, 15);

        for (j = 0; j < 4; j++) {
            if (mask_bits & (1 << j))
                cb.blocks[i].pixels[j] = color1;
            else
                cb.blocks[i].pixels[j] = color0;
        }
    }
    return cb;
}
