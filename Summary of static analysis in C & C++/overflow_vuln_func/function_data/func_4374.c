static int get_high_utility_cell(elbg_data *elbg)
{
    int i=0;
    /
    int r = av_lfg_get(elbg->rand_state)%elbg->utility_inc[elbg->numCB-1] + 1;
    while (elbg->utility_inc[i] < r)
        i++;

    av_assert2(elbg->cells[i]);

    return i;
}
