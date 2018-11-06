static int decode_pal(MSS12Context *ctx, ArithCoder *acoder)
{
    int i, ncol, r, g, b;
    uint32_t *pal = ctx->pal + 256 - ctx->free_colours;

    if (!ctx->free_colours)
        return 0;

    ncol = arith_get_number(acoder, ctx->free_colours + 1);
    for (i = 0; i < ncol; i++) {
        r = arith_get_bits(acoder, 8);
        g = arith_get_bits(acoder, 8);
        b = arith_get_bits(acoder, 8);
        *pal++ = (0xFF << 24) | (r << 16) | (g << 8) | b;
    }

    return !!ncol;
}
