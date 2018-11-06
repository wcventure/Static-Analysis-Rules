uint_fast16_t float32_to_uint16_round_to_zero(float32 a STATUS_PARAM)
{
    int64_t v;
    uint_fast16_t res;

    v = float32_to_int64_round_to_zero(a STATUS_VAR);
    if (v < 0) {
        res = 0;
        float_raise( float_flag_invalid STATUS_VAR);
    } else if (v > 0xffff) {
        res = 0xffff;
        float_raise( float_flag_invalid STATUS_VAR);
    } else {
        res = v;
    }
    return res;
}
