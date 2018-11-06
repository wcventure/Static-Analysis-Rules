static inline void gen_addr_imm_index (DisasContext *ctx)
{
    target_long simm = SIMM(ctx->opcode);

    if (rA(ctx->opcode) == 0) {
        gen_op_set_T0(simm);
    } else {
        gen_op_load_gpr_T0(rA(ctx->opcode));
        if (likely(simm != 0))
            gen_op_addi(simm);
    }
}
