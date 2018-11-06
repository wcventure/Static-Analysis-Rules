static int cmp_int(const void *p1, const void *p2)
{
    int left = *(const int *)p1;
    int right = *(const int *)p2;

    return ((left > right) - (left < right));
}
