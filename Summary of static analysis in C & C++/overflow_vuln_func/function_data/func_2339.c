void do_store_601_batu (int nr)
{
    do_store_ibatu(env, nr, T0);
    env->DBAT[0][nr] = env->IBAT[0][nr];
    env->DBAT[1][nr] = env->IBAT[1][nr];
}
