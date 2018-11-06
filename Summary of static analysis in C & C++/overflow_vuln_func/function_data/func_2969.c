float16 float32_to_float16(float32 a, flag ieee STATUS_PARAM)
{
    flag aSign;
    int16 aExp;
    bits32 aSig;
    bits32 mask;
    bits32 increment;
    int8 roundingMode;
    a = float32_squash_input_denormal(a STATUS_VAR);

    aSig = extractFloat32Frac( a );
    aExp = extractFloat32Exp( a );
    aSign = extractFloat32Sign( a );
    if ( aExp == 0xFF ) {
        if (aSig) {
            /
            float32ToCommonNaN(a STATUS_VAR);
            aSig |= 0x00400000;
        }
        return packFloat16(aSign, 0x1f, aSig >> 13);
    }
    if (aExp == 0 && aSign == 0) {
        return packFloat16(aSign, 0, 0);
    }
    /
    aSig |= 0x00800000;
    aExp -= 0x7f;
    if (aExp < -14) {
        mask = 0x007fffff;
        if (aExp < -24) {
            aExp = -25;
        } else {
            mask >>= 24 + aExp;
        }
    } else {
        mask = 0x00001fff;
    }
    if (aSig & mask) {
        float_raise( float_flag_underflow STATUS_VAR );
        roundingMode = STATUS(float_rounding_mode);
        switch (roundingMode) {
        case float_round_nearest_even:
            increment = (mask + 1) >> 1;
            if ((aSig & mask) == increment) {
                increment = aSig & (increment << 1);
            }
            break;
        case float_round_up:
            increment = aSign ? 0 : mask;
            break;
        case float_round_down:
            increment = aSign ? mask : 0;
            break;
        default: /
            increment = 0;
            break;
        }
        aSig += increment;
        if (aSig >= 0x01000000) {
            aSig >>= 1;
            aExp++;
        }
    } else if (aExp < -14
          && STATUS(float_detect_tininess) == float_tininess_before_rounding) {
        float_raise( float_flag_underflow STATUS_VAR);
    }

    if (ieee) {
        if (aExp > 15) {
            float_raise( float_flag_overflow | float_flag_inexact STATUS_VAR);
            return packFloat16(aSign, 0x1f, 0);
        }
    } else {
        if (aExp > 16) {
            float_raise( float_flag_overflow | float_flag_inexact STATUS_VAR);
            return packFloat16(aSign, 0x1f, 0x3ff);
        }
    }
    if (aExp < -24) {
        return packFloat16(aSign, 0, 0);
    }
    if (aExp < -14) {
        aSig >>= -14 - aExp;
        aExp = -14;
    }
    return packFloat16(aSign, aExp + 14, aSig >> 13);
}
