void OPPROTO op_idivb_AL_T0(void)
{
    int num, den, q, r;

    num = (int16_t)EAX;
    den = (int8_t)T0;
    if (den == 0) {
        raise_exception(EXCP00_DIVZ);
    }
    q = (num / den) & 0xff;
    r = (num % den) & 0xff;
    EAX = (EAX & ~0xffff) | (r << 8) | q;
}
