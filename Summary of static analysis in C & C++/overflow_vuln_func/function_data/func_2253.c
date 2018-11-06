PPC_OP(setlr)
{
    regs->lr = PARAM1;
    RETURN();
}
