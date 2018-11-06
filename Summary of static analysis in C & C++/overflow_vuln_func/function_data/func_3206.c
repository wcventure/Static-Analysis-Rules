static av_always_inline int lcg_random(int previous_val)
{
    return previous_val * 1664525 + 1013904223;
}
