static inline int coeff_unpack_golomb(GetBitContext *gb, int qfactor, int qoffset)
{
    int coeff = dirac_get_se_golomb(gb);
    const int sign = FFSIGN(coeff);
    if (coeff)
        coeff = sign*((sign * coeff * qfactor + qoffset) >> 2);
    return coeff;
}
