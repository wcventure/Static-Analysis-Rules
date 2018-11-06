PPC_OP(btest_T1) 
{
    if (T0) {
        regs->nip = T1 & ~3;
    } else {
        regs->nip = PARAM1;
    }
    RETURN();
}
