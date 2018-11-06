static inline void gen_set_Rc0 (DisasContext *ctx)
{
    gen_op_cmpi(0);
    gen_op_set_Rc0();
}
