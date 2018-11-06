static void sr_1d97_float(float *p, int i0, int i1)
{
    int i;

    if (i1 <= i0 + 1) {
        if (i0 == 1)
            p[1] *= F_LFTG_K/2;
        else
            p[0] *= F_LFTG_X/2;
        return;
    }

    extend97_float(p, i0, i1);

    for (i = i0 / 2 - 1; i < i1 / 2 + 2; i++)
        p[2 * i]     -= F_LFTG_DELTA * (p[2 * i - 1] + p[2 * i + 1]);
    /
    for (i = i0 / 2 - 1; i < i1 / 2 + 1; i++)
        p[2 * i + 1] -= F_LFTG_GAMMA * (p[2 * i]     + p[2 * i + 2]);
    /
    for (i = i0 / 2; i < i1 / 2 + 1; i++)
        p[2 * i]     += F_LFTG_BETA  * (p[2 * i - 1] + p[2 * i + 1]);
    /
    for (i = i0 / 2; i < i1 / 2; i++)
        p[2 * i + 1] += F_LFTG_ALPHA * (p[2 * i]     + p[2 * i + 2]);
}
