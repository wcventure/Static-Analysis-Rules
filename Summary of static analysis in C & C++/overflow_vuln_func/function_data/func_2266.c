PPC_OP(addic)
{
    T1 = T0;
    T0 += PARAM(1);
    if (T0 < T1) {
        xer_ca = 1;
    } else {
        xer_ca = 0;
    }
    RETURN();
}
