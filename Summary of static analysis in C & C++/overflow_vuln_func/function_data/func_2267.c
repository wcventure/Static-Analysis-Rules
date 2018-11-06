PPC_OP(addme)
{
    T1 = T0;
    T0 += xer_ca + (-1);
    if (T1 != 0)
        xer_ca = 1;
    RETURN();
}
