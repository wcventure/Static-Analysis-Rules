float32 float32_scalbn( float32 a, int n STATUS_PARAM )
{
    flag aSign;
    int16 aExp;
    uint32_t aSig;

    a = float32_squash_input_denormal(a STATUS_VAR);
    aSig = extractFloat32Frac( a );
    aExp = extractFloat32Exp( a );
    aSign = extractFloat32Sign( a );

    if ( aExp == 0xFF ) {
        return a;
    }
    if ( aExp != 0 )
        aSig |= 0x00800000;
    else if ( aSig == 0 )
        return a;

    aExp += n - 1;
    aSig <<= 7;
    return normalizeRoundAndPackFloat32( aSign, aExp, aSig STATUS_VAR );
}
