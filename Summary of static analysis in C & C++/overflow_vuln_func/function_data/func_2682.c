TCGOp *tcg_op_insert_before(TCGContext *s, TCGOp *old_op,
                            TCGOpcode opc, int nargs)
{
    int oi = s->gen_next_op_idx;
    int prev = old_op->prev;
    int next = old_op - s->gen_op_buf;
    TCGOp *new_op;

    tcg_debug_assert(oi < OPC_BUF_SIZE);
    s->gen_next_op_idx = oi + 1;

    new_op = &s->gen_op_buf[oi];
    *new_op = (TCGOp){
        .opc = opc,
        .prev = prev,
        .next = next
    };
    s->gen_op_buf[prev].next = oi;
    old_op->prev = oi;

    return new_op;
}
