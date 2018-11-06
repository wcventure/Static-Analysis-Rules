static void calc_sums(int pmin, int pmax, uint32_t *data, int n, int pred_order,
                      uint32_t sums[][MAX_PARTITIONS])
{
    int i, j;
    int parts;
    uint32_t *res, *res_end;

    /
    parts   = (1 << pmax);
    res     = &data[pred_order];
    res_end = &data[n >> pmax];
    for (i = 0; i < parts; i++) {
        uint32_t sum = 0;
        while (res < res_end)
            sum += *(res++);
        sums[pmax][i] = sum;
        res_end += n >> pmax;
    }
    /
    for (i = pmax - 1; i >= pmin; i--) {
        parts = (1 << i);
        for (j = 0; j < parts; j++)
            sums[i][j] = sums[i+1][2*j] + sums[i+1][2*j+1];
    }
}
