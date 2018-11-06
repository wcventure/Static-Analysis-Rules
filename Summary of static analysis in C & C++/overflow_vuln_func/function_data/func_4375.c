static void evaluate_utility_inc(elbg_data *elbg)
{
    int i, inc=0;

    for (i=0; i < elbg->numCB; i++) {
        if (elbg->numCB*elbg->utility[i] > elbg->error)
            inc += elbg->utility[i];
        elbg->utility_inc[i] = inc;
    }
}
