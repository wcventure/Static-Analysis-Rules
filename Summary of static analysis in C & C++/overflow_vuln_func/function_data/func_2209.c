static void gen_advance_ccount(DisasContext *dc)
{
    gen_advance_ccount_cond(dc);
    dc->ccount_delta = 0;
}
