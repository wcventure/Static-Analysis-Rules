PPC_OP(cmp)
{
    if (Ts0 < Ts1) {
        T0 = 0x08;
    } else if (Ts0 > Ts1) {
        T0 = 0x04;
    } else {
        T0 = 0x02;
    }
    RETURN();
}
