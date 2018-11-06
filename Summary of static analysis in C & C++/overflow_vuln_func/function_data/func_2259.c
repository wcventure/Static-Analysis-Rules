PPC_OP(test_ctrz)
{
    T0 = (regs->ctr == 0);
    RETURN();
}
