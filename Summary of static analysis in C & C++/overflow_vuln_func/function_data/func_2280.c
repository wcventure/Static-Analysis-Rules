PPC_OP(subfic)
{
    T0 = PARAM(1) + ~T0 + 1;
    if (T0 <= PARAM(1)) {
        xer_ca = 1;
    } else {
        xer_ca = 0;
    }
    RETURN();
}
