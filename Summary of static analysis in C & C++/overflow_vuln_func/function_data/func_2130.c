void OPPROTO op_divw_AX_T0(void)
{
    unsigned int num, den, q, r;

    num = (EAX & 0xffff) | ((EDX & 0xffff) << 16);
    den = (T0 & 0xffff);
    if (den == 0) {
        raise_exception(EXCP00_DIVZ);
    }
    q = (num / den) & 0xffff;
    r = (num % den) & 0xffff;
    EAX = (EAX & ~0xffff) | q;
    EDX = (EDX & ~0xffff) | r;
}
