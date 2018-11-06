PPC_OP(addze)
{
    T1 = T0;
    T0 += xer_ca;
    if (T0 < T1) {
        xer_ca = 1;
    } else {
        xer_ca = 0;
    }
    RETURN();
}
