static void dnxhd_decode_dct_block_8(const DNXHDContext *ctx,
                                     RowContext *row, int n)
{
    dnxhd_decode_dct_block(ctx, row, n, 4, 32, 6);
}
