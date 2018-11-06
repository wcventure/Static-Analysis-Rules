int32 float64_to_int32_round_to_zero( float64 a STATUS_PARAM )
{
    flag aSign;
    int16 aExp, shiftCount;
    uint64_t aSig, savedASig;
    int32 z;
    a = float64_squash_input_denormal(a STATUS_VAR);

    aSig = extractFloat64Frac( a );
    aExp = extractFloat64Exp( a );
    aSign = extractFloat64Sign( a );
    if ( 0x41E < aExp ) {
        if ( ( aExp == 0x7FF ) && aSig ) aSign = 0;
        goto invalid;
    }
    else if ( aExp < 0x3FF ) {
        if ( aExp || aSig ) STATUS(float_exception_flags) |= float_flag_inexact;
        return 0;
    }
    aSig |= LIT64( 0x0010000000000000 );
    shiftCount = 0x433 - aExp;
    savedASig = aSig;
    aSig >>= shiftCount;
    z = aSig;
    if ( aSign ) z = - z;
    if ( ( z < 0 ) ^ aSign ) {
 invalid:
        float_raise( float_flag_invalid STATUS_VAR);
        return aSign ? (int32_t) 0x80000000 : 0x7FFFFFFF;
    }
    if ( ( aSig<<shiftCount ) != savedASig ) {
        STATUS(float_exception_flags) |= float_flag_inexact;
    }
    return z;

}
