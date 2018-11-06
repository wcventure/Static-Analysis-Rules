PPC_OP(setcrfbit)
{
    T1 = (T1 & PARAM(1)) | (T0 << PARAM(2)); 
    RETURN();
}
