static always_inline void gen_op_subfc (void)
{
    gen_op_subf();
    gen_op_check_subfc();
}
static always_inline void gen_op_subfco (void)
{
    gen_op_move_T2_T0();
    gen_op_subf();
    gen_op_check_subfc();
    gen_op_check_subfo();
}
#if defined(TARGET_PPC64)
static always_inline void gen_op_subfc_64 (void)
{
    gen_op_subf();
    gen_op_check_subfc_64();
}
static always_inline void gen_op_subfco_64 (void)
{
    gen_op_move_T2_T0();
    gen_op_subf();
    gen_op_check_subfc_64();
    gen_op_check_subfo_64();
}
