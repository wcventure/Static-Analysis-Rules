static uint32_t calc_rice_params(RiceContext *rc, int pmin, int pmax,
                                 int32_t *data, int n, int pred_order)
{
    int i;
    uint32_t bits[MAX_PARTITION_ORDER+1];
    int opt_porder;
    RiceContext tmp_rc;
    uint32_t *udata;
    uint32_t sums[MAX_PARTITION_ORDER+1][MAX_PARTITIONS];

    assert(pmin >= 0 && pmin <= MAX_PARTITION_ORDER);
    assert(pmax >= 0 && pmax <= MAX_PARTITION_ORDER);
    assert(pmin <= pmax);

    udata = av_malloc(n * sizeof(uint32_t));
    for (i = 0; i < n; i++)
        udata[i] = (2*data[i]) ^ (data[i]>>31);

    calc_sums(pmin, pmax, udata, n, pred_order, sums);

    opt_porder = pmin;
    bits[pmin] = UINT32_MAX;
    for (i = pmin; i <= pmax; i++) {
        bits[i] = calc_optimal_rice_params(&tmp_rc, i, sums[i], n, pred_order);
        if (bits[i] <= bits[opt_porder]) {
            opt_porder = i;
            *rc = tmp_rc;
        }
    }

    av_freep(&udata);
    return bits[opt_porder];
}
