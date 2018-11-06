static av_always_inline int even(uint64_t layout)
{
    return (!layout || (layout & (layout - 1)));
}
