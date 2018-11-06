static void dnxhd_decode_dct_block_10(const DNXHDContext *ctx,
                                      RowContext *row, int n)
{
    dnxhd_decode_dct_block(ctx, row, n, 6, 8, 4);
}
