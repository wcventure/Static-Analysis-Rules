static uint32_t calc_optimal_rice_params(RiceContext *rc, int porder,
                                         uint32_t *sums, int n, int pred_order)
{
    int i;
    int k, cnt, part;
    uint32_t all_bits;

    part     = (1 << porder);
    all_bits = 4 * part;

    cnt = (n >> porder) - pred_order;
    for (i = 0; i < part; i++) {
        k = find_optimal_param(sums[i], cnt);
        rc->params[i] = k;
        all_bits += rice_encode_count(sums[i], cnt, k);
        cnt = n >> porder;
    }

    rc->porder = porder;

    return all_bits;
}
