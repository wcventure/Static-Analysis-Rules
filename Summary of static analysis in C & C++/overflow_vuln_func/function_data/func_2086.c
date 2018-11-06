#ifdef FLOATX80
floatx80 floatx80_scalbn( floatx80 a, int n STATUS_PARAM )
{
    flag aSign;
    int16 aExp;
    uint64_t aSig;

    aSig = extractFloatx80Frac( a );
    aExp = extractFloatx80Exp( a );
    aSign = extractFloatx80Sign( a );

    if ( aExp == 0x7FF ) {
        return a;
    }
    if (aExp == 0 && aSig == 0)
        return a;

    aExp += n;
    return normalizeRoundAndPackFloatx80( STATUS(floatx80_rounding_precision),
                                          aSign, aExp, aSig, 0 STATUS_VAR );
}
