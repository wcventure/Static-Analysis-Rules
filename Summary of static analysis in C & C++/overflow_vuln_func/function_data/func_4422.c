void *av_fast_realloc(void *ptr, unsigned int *size, unsigned int min_size)
{
    if(min_size < *size)
        return ptr;

    *size= FFMAX(17*min_size/16 + 32, min_size);

    return av_realloc(ptr, *size);
}
