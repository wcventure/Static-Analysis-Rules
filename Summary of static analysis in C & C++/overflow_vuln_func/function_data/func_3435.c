static void vc1_v_overlap_c(uint8_t* src, int stride)
{
    int i;
    int a, b, c, d;
    int d1, d2;
    int rnd = 1;
    for(i = 0; i < 8; i++) {
        a = src[-2*stride];
        b = src[-stride];
        c = src[0];
        d = src[stride];
        d1 = (a - d + 3 + rnd) >> 3;
        d2 = (a - d + b - c + 4 - rnd) >> 3;

        src[-2*stride] = a - d1;
        src[-stride] = b - d2;
        src[0] = c + d2;
        src[stride] = d + d1;
        src++;
        rnd = !rnd;
    }
}
