static inline void RET_STOP (DisasContext *ctx)
{
    gen_op_update_nip((ctx)->nip);
    ctx->exception = EXCP_MTMSR;
}
