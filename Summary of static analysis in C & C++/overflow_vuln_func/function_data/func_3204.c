static int color_distance(uint32_t a, uint32_t b)
{
    int r = 0, d, i;

    for (i = 0; i < 32; i += 8) {
        d = ((a >> i) & 0xFF) - ((b >> i) & 0xFF);
        r += d * d;
    }
    return r;
}
