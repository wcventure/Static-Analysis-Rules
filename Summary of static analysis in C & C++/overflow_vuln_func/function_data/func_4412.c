static av_cold void construct_perm_table(TwinContext *tctx,enum FrameType ftype)
{
    int block_size;
    const ModeTab *mtab = tctx->mtab;
    int size = tctx->avctx->channels*mtab->fmode[ftype].sub;
    int16_t *tmp_perm = (int16_t *) tctx->tmp_buf;

    if (ftype == FT_PPC) {
        size  = tctx->avctx->channels;
        block_size = mtab->ppc_shape_len;
    } else
        block_size = mtab->size / mtab->fmode[ftype].sub;

    permutate_in_line(tmp_perm, tctx->n_div[ftype], size,
                      block_size, tctx->length[ftype],
                      tctx->length_change[ftype], ftype);

    transpose_perm(tctx->permut[ftype], tmp_perm, tctx->n_div[ftype],
                   tctx->length[ftype], tctx->length_change[ftype]);

    linear_perm(tctx->permut[ftype], tctx->permut[ftype], size,
                size*block_size);
}
