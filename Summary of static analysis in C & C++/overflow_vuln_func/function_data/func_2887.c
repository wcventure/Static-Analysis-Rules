int32 float128_to_int32_round_to_zero( float128 a STATUS_PARAM )
{
    flag aSign;
    int32 aExp, shiftCount;
    uint64_t aSig0, aSig1, savedASig;
    int32 z;

    aSig1 = extractFloat128Frac1( a );
    aSig0 = extractFloat128Frac0( a );
    aExp = extractFloat128Exp( a );
    aSign = extractFloat128Sign( a );
    aSig0 |= ( aSig1 != 0 );
    if ( 0x401E < aExp ) {
        if ( ( aExp == 0x7FFF ) && aSig0 ) aSign = 0;
        goto invalid;
    }
    else if ( aExp < 0x3FFF ) {
        if ( aExp || aSig0 ) STATUS(float_exception_flags) |= float_flag_inexact;
        return 0;
    }
    aSig0 |= LIT64( 0x0001000000000000 );
    shiftCount = 0x402F - aExp;
    savedASig = aSig0;
    aSig0 >>= shiftCount;
    z = aSig0;
    if ( aSign ) z = - z;
    if ( ( z < 0 ) ^ aSign ) {
 invalid:
        float_raise( float_flag_invalid STATUS_VAR);
        return aSign ? (int32_t) 0x80000000 : 0x7FFFFFFF;
    }
    if ( ( aSig0<<shiftCount ) != savedASig ) {
        STATUS(float_exception_flags) |= float_flag_inexact;
    }
    return z;

}
