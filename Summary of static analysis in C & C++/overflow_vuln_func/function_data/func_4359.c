static int cmp(const void *a, const void *b)
{
    const double va = *(const double *)a, vb = *(const double *)b;
    return va < vb ? -1 : ( va > vb ? 1 : 0 );
}
