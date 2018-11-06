PPC_OP(cmpli)
{
    if (T0 < PARAM(1)) {
        T0 = 0x08;
    } else if (T0 > PARAM(1)) {
        T0 = 0x04;
    } else {
        T0 = 0x02;
    }
    RETURN();
}
