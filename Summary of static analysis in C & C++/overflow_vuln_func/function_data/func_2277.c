PPC_OP(subfc)
{
    T0 = T1 - T0;
    if (T0 <= T1) {
        xer_ca = 1;
    } else {
        xer_ca = 0;
    }
    RETURN();
}
