PPC_OP(srw)
{
    if (T1 & 0x20) {
        T0 = 0;
    } else {
        T0 = T0 >> T1;
    }
    RETURN();
}
