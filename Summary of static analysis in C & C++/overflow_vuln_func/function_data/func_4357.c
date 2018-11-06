static int compare_int64(const void *a, const void *b)
{
    int64_t va = *(int64_t *)a, vb = *(int64_t *)b;
    return va < vb ? -1 : va > vb ? +1 : 0;
}
