void OPPROTO op_divb_AL_T0(void)
{
    unsigned int num, den, q, r;

    num = (EAX & 0xffff);
    den = (T0 & 0xff);
    if (den == 0) {
        raise_exception(EXCP00_DIVZ);
    }
    q = (num / den) & 0xff;
    r = (num % den) & 0xff;
    EAX = (EAX & ~0xffff) | (r << 8) | q;
}
