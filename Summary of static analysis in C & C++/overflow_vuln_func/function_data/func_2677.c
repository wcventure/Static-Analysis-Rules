static inline TCGOp *tcg_emit_op(TCGOpcode opc)
{
    TCGContext *ctx = tcg_ctx;
    int oi = ctx->gen_next_op_idx;
    int ni = oi + 1;
    int pi = oi - 1;
    TCGOp *op = &ctx->gen_op_buf[oi];

    tcg_debug_assert(oi < OPC_BUF_SIZE);
    ctx->gen_op_buf[0].prev = oi;
    ctx->gen_next_op_idx = ni;

    memset(op, 0, offsetof(TCGOp, args));
    op->opc = opc;
    op->prev = pi;
    op->next = ni;

    return op;
}
