static int vlc_decode_block(MimicContext *ctx, int num_coeffs, int qscale)
{
    int16_t *block = ctx->dct_block;
    unsigned int pos;

    ctx->bdsp.clear_block(block);

    block[0] = get_bits(&ctx->gb, 8) << 3;

    for (pos = 1; pos < num_coeffs; pos++) {
        uint32_t vlc, num_bits;
        int value;
        int coeff;

        vlc = get_vlc2(&ctx->gb, ctx->vlc.table, ctx->vlc.bits, 3);
        if (!vlc) /
            return 0;
        if (vlc == -1)
            return AVERROR_INVALIDDATA;

        /
        pos     += vlc & 15; // pos_add
        num_bits = vlc >> 4; // num_bits

        if (pos >= 64)
            return AVERROR_INVALIDDATA;

        value = get_bits(&ctx->gb, num_bits);

        /

        coeff = vlcdec_lookup[num_bits][value];
        if (pos < 3)
            coeff *= 16;
        else /
            coeff = (coeff * qscale) / 1001;

        block[ctx->scantable.permutated[pos]] = coeff;
    }

    return 0;
}
