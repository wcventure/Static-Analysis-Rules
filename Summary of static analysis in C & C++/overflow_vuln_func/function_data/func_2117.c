static always_inline void gen_op_subfeo (void)
{
    gen_op_move_T2_T0();
    gen_op_subfe();
    gen_op_check_subfo();
}
