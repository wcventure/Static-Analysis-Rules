static inline int ff_fast_malloc(void *ptr, unsigned int *size, size_t min_size, int zero_realloc)
{
    void **p = ptr;
    if (min_size <= *size && *p)
        return 0;
    min_size = FFMAX(17 * min_size / 16 + 32, min_size);
    av_free(*p);
    *p = zero_realloc ? av_mallocz(min_size) : av_malloc(min_size);
    if (!*p)
        min_size = 0;
    *size = min_size;
    return 1;
}
