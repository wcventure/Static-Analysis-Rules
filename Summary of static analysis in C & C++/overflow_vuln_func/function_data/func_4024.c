static int diff_C(unsigned char *old, unsigned char *new, int os, int ns)
{
    int x, y, d=0;
    for (y = 8; y; y--) {
        for (x = 8; x; x--) {
            d += abs(new[x] - old[x]);
        }
        new += ns;
        old += os;
    }
    return d;
}
