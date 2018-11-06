TCGOp *tcg_op_insert_after(TCGContext *s, TCGOp *old_op,
                           TCGOpcode opc, int nargs)
{
    int oi = s->gen_next_op_idx;
    int prev = old_op - s->gen_op_buf;
    int next = old_op->next;
    TCGOp *new_op;

    tcg_debug_assert(oi < OPC_BUF_SIZE);
    s->gen_next_op_idx = oi + 1;

    new_op = &s->gen_op_buf[oi];
    *new_op = (TCGOp){
        .opc = opc,
        .prev = prev,
        .next = next
    };
    s->gen_op_buf[next].prev = oi;
    old_op->next = oi;

    return new_op;
}
