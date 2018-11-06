float64 float64_scalbn( float64 a, int n STATUS_PARAM )
{
    flag aSign;
    int16 aExp;
    uint64_t aSig;

    a = float64_squash_input_denormal(a STATUS_VAR);
    aSig = extractFloat64Frac( a );
    aExp = extractFloat64Exp( a );
    aSign = extractFloat64Sign( a );

    if ( aExp == 0x7FF ) {
        return a;
    }
    if ( aExp != 0 )
        aSig |= LIT64( 0x0010000000000000 );
    else if ( aSig == 0 )
        return a;

    aExp += n - 1;
    aSig <<= 10;
    return normalizeRoundAndPackFloat64( aSign, aExp, aSig STATUS_VAR );
}
