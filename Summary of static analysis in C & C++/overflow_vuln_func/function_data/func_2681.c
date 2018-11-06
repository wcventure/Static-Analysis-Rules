void tcg_op_remove(TCGContext *s, TCGOp *op)
{
    int next = op->next;
    int prev = op->prev;

    /
    tcg_debug_assert(op != &s->gen_op_buf[0]);

    s->gen_op_buf[next].prev = prev;
    s->gen_op_buf[prev].next = next;

    memset(op, 0, sizeof(*op));

#ifdef CONFIG_PROFILER
    atomic_set(&s->prof.del_op_count, s->prof.del_op_count + 1);
#endif
}
