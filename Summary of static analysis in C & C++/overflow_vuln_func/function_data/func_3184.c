static uint32_t parse_peak(const uint8_t *peak)
{
    int64_t val = 0;
    int64_t scale = 1;

    if (!peak)
        return 0;

    peak += strspn(peak, " \t");

    if (peak[0] == '1' && peak[1] == '.')
        return UINT32_MAX;
    else if (!(peak[0] == '0' && peak[1] == '.'))
        return 0;

    peak += 2;

    while (av_isdigit(*peak)) {
        int digit = *peak - '0';

        if (scale > INT64_MAX / 10)
            break;

        val    = 10 * val + digit;
        scale *= 10;

        peak++;
    }

    return av_rescale(val, UINT32_MAX, scale);
}
