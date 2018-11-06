static void arm_tr_insn_start(DisasContextBase *dcbase, CPUState *cpu)
{
    DisasContext *dc = container_of(dcbase, DisasContext, base);

    dc->insn_start_idx = tcg_op_buf_count();
    tcg_gen_insn_start(dc->pc,
                       (dc->condexec_cond << 4) | (dc->condexec_mask >> 1),
                       0);
}
