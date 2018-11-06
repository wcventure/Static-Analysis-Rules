PPC_OP(b_T1)
{
    regs->nip = T1 & ~3;
    RETURN();
}
