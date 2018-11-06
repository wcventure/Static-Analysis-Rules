PPC_OP(subfme)
{
    T0 = ~T0 + xer_ca - 1;

    if (T0 != -1)
        xer_ca = 1;
    RETURN();
}
