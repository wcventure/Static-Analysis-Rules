static int16_t square_root(int val)
{
    return (ff_sqrt(val << 1) >> 1) & (~1);
}
