#ifdef FLOAT128
float128 float128_scalbn( float128 a, int n STATUS_PARAM )
{
    flag aSign;
    int32 aExp;
    uint64_t aSig0, aSig1;

    aSig1 = extractFloat128Frac1( a );
    aSig0 = extractFloat128Frac0( a );
    aExp = extractFloat128Exp( a );
    aSign = extractFloat128Sign( a );
    if ( aExp == 0x7FFF ) {
        return a;
    }
    if ( aExp != 0 )
        aSig0 |= LIT64( 0x0001000000000000 );
    else if ( aSig0 == 0 && aSig1 == 0 )
        return a;

    aExp += n - 1;
    return normalizeRoundAndPackFloat128( aSign, aExp, aSig0, aSig1
                                          STATUS_VAR );

}
