static always_inline void gen_op_subfeo_64 (void)
{
    gen_op_move_T2_T0();
    gen_op_subfe_64();
    gen_op_check_subfo_64();
}
