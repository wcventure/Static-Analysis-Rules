static int calc_add_mv(RV34DecContext *r, int dir, int val)
{
    int mul = dir ? -r->mv_weight2 : r->mv_weight1;

    return (val * mul + 0x2000) >> 14;
}
