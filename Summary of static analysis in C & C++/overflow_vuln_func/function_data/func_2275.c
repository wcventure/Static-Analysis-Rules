PPC_OP(neg)
{
    if (T0 != 0x80000000) {
        T0 = -Ts0;
    }
    RETURN();
}
