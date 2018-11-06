PPC_OP(divw)
{
    if ((Ts0 == INT32_MIN && Ts1 == -1) || Ts1 == 0) {
        T0 = (int32_t)((-1) * (T0 >> 31));
    } else {
        T0 = (Ts0 / Ts1);
    }
    RETURN();
}
