PPC_OP(addc)
{
    T2 = T0;
    T0 += T1;
    if (T0 < T2) {
        xer_ca = 1;
    } else {
        xer_ca = 0;
    }
    RETURN();
}
