PPC_OP(srawi)
{
    T1 = T0;
    T0 = (Ts0 >> PARAM(1));
    if (Ts1 < 0 && (Ts1 & PARAM(2)) != 0) {
        xer_ca = 1;
    } else {
        xer_ca = 0;
    }
    RETURN();
}
