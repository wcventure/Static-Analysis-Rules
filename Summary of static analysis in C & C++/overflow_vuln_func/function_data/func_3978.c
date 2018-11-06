static inline int gsm_mult(int a, int b)
{
    return (a * b + (1 << 14)) >> 15;
}
