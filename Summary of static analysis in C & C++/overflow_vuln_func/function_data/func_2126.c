void helper_divq_EAX_T0(void)
{
    uint64_t r0, r1;
    if (T0 == 0) {
        raise_exception(EXCP00_DIVZ);
    }
    r0 = EAX;
    r1 = EDX;
    div64(&r0, &r1, T0);
    EAX = r0;
    EDX = r1;
}
