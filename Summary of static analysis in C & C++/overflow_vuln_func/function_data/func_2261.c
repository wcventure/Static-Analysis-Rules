PPC_OP(test_ctrz_false)
{
    T0 = (regs->ctr == 0 && (T0 & PARAM(1)) == 0);
    RETURN();
}
