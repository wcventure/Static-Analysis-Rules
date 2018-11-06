static void decode_block_params(DiracContext *s, DiracArith arith[8], DiracBlock *block,
                                int stride, int x, int y)
{
    int i;

    block->ref  = pred_block_mode(block, stride, x, y, DIRAC_REF_MASK_REF1);
    block->ref ^= dirac_get_arith_bit(arith, CTX_PMODE_REF1);

    if (s->num_refs == 2) {
        block->ref |= pred_block_mode(block, stride, x, y, DIRAC_REF_MASK_REF2);
        block->ref ^= dirac_get_arith_bit(arith, CTX_PMODE_REF2) << 1;
    }

    if (!block->ref) {
        pred_block_dc(block, stride, x, y);
        for (i = 0; i < 3; i++)
            block->u.dc[i] += dirac_get_arith_int(arith+1+i, CTX_DC_F1, CTX_DC_DATA);
        return;
    }

    if (s->globalmc_flag) {
        block->ref |= pred_block_mode(block, stride, x, y, DIRAC_REF_MASK_GLOBAL);
        block->ref ^= dirac_get_arith_bit(arith, CTX_GLOBAL_BLOCK) << 2;
    }

    for (i = 0; i < s->num_refs; i++)
        if (block->ref & (i+1)) {
            if (block->ref & DIRAC_REF_MASK_GLOBAL) {
                global_mv(s, block, x, y, i);
            } else {
                pred_mv(block, stride, x, y, i);
                block->u.mv[i][0] += dirac_get_arith_int(arith + 4 + 2 * i, CTX_MV_F1, CTX_MV_DATA);
                block->u.mv[i][1] += dirac_get_arith_int(arith + 5 + 2 * i, CTX_MV_F1, CTX_MV_DATA);
            }
        }
}
