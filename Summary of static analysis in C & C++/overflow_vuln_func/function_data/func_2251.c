PPC_OP(update_nip)
{
    env->nip = PARAM(1);
    RETURN();
}
