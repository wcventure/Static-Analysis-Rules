void op_subo (void)
{
    target_ulong tmp;

    tmp = T0;
    T0 = (int32_t)T0 - (int32_t)T1;
    if (!((T0 >> 31) ^ (T1 >> 31) ^ (tmp >> 31))) {
        CALL_FROM_TB1(do_raise_exception_direct, EXCP_OVERFLOW);
    }
    RETURN();
}
