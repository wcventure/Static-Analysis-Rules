int32 floatx80_to_int32_round_to_zero( floatx80 a STATUS_PARAM )
{
    flag aSign;
    int32 aExp, shiftCount;
    uint64_t aSig, savedASig;
    int32 z;

    aSig = extractFloatx80Frac( a );
    aExp = extractFloatx80Exp( a );
    aSign = extractFloatx80Sign( a );
    if ( 0x401E < aExp ) {
        if ( ( aExp == 0x7FFF ) && (uint64_t) ( aSig<<1 ) ) aSign = 0;
        goto invalid;
    }
    else if ( aExp < 0x3FFF ) {
        if ( aExp || aSig ) STATUS(float_exception_flags) |= float_flag_inexact;
        return 0;
    }
    shiftCount = 0x403E - aExp;
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
