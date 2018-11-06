static always_inline void gen_op_subfo (void)
{
    gen_op_move_T2_T0();
    gen_op_subf();
    gen_op_check_subfo();
}
