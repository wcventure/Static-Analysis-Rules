PPC_OP(mulhwu)
{
    T0 = ((uint64_t)T0 * (uint64_t)T1) >> 32;
    RETURN();
}
