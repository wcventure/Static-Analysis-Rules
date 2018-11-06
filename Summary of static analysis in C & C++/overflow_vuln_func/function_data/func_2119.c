static always_inline void gen_405_mulladd_insn (DisasContext *ctx,
                                                int opc2, int opc3,
                                                int ra, int rb, int rt, int Rc)
{
    gen_op_load_gpr_T0(ra);
    gen_op_load_gpr_T1(rb);
    switch (opc3 & 0x0D) {
    case 0x05:
        /
        /
        /
        /
        /
        gen_op_405_mulchw();
        break;
    case 0x04:
        /
        /
        /
        gen_op_405_mulchwu();
        break;
    case 0x01:
        /
        /
        /
        /
        /
        gen_op_405_mulhhw();
        break;
    case 0x00:
        /
        /
        /
        gen_op_405_mulhhwu();
        break;
    case 0x0D:
        /
        /
        /
        /
        /
        gen_op_405_mullhw();
        break;
    case 0x0C:
        /
        /
        /
        gen_op_405_mullhwu();
        break;
    }
    if (opc2 & 0x02) {
        /
        gen_op_neg();
    }
    if (opc2 & 0x04) {
        /
        gen_op_load_gpr_T2(rt);
        gen_op_move_T1_T0();
        gen_op_405_add_T0_T2();
    }
    if (opc3 & 0x10) {
        /
        if (opc3 & 0x01)
            gen_op_405_check_ov();
        else
            gen_op_405_check_ovu();
    }
    if (opc3 & 0x02) {
        /
        if (opc3 & 0x01)
            gen_op_405_check_sat();
        else
            gen_op_405_check_satu();
    }
    gen_op_store_T0_gpr(rt);
    if (unlikely(Rc) != 0) {
        /
        gen_set_Rc0(ctx);
    }
}
