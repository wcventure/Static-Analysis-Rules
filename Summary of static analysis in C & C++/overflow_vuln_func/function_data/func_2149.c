uint32 float32_to_uint32( float32 a STATUS_PARAM )
{
    int64_t v;
    uint32 res;

    v = float32_to_int64(a STATUS_VAR);
    if (v < 0) {
        res = 0;
        float_raise( float_flag_invalid STATUS_VAR);
    } else if (v > 0xffffffff) {
        res = 0xffffffff;
        float_raise( float_flag_invalid STATUS_VAR);
    } else {
        res = v;
    }
    return res;
}
