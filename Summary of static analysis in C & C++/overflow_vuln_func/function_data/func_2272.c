PPC_OP(mulhw)
{
    T0 = ((int64_t)Ts0 * (int64_t)Ts1) >> 32;
    RETURN();
}
