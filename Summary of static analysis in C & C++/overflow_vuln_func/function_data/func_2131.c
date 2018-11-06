void OPPROTO op_idivw_AX_T0(void)
{
    int num, den, q, r;

    num = (EAX & 0xffff) | ((EDX & 0xffff) << 16);
    den = (int16_t)T0;
    if (den == 0) {
        raise_exception(EXCP00_DIVZ);
    }
    q = (num / den) & 0xffff;
    r = (num % den) & 0xffff;
    EAX = (EAX & ~0xffff) | q;
    EDX = (EDX & ~0xffff) | r;
}
