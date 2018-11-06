void OPPROTO op_subfme (void)
{
    T0 = ~T0 + xer_ca - 1;
    if (likely((uint32_t)T0 != (uint32_t)-1))
        xer_ca = 1;
    RETURN();
}
