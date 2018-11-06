PPC_OP(check_reservation)
{
    if ((uint32_t)env->reserve == (uint32_t)(T0 & ~0x00000003))
        env->reserve = -1;
    RETURN();
}
