static void aarch64_tr_insn_start(DisasContextBase *dcbase, CPUState *cpu)
{
    DisasContext *dc = container_of(dcbase, DisasContext, base);

    dc->insn_start_idx = tcg_op_buf_count();
    tcg_gen_insn_start(dc->pc, 0, 0);
}
