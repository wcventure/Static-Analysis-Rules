static int32_t parse_gain(const char *gain)
{
    char *fraction;
    int  scale = 10000;
    int32_t mb = 0;
    int sign   = 1;
    int db;

    if (!gain)
        return INT32_MIN;

    gain += strspn(gain, " \t");

    if (*gain == '-')
        sign = -1;

    db = strtol(gain, &fraction, 0);
    if (*fraction++ == '.') {
        while (av_isdigit(*fraction) && scale) {
            mb += scale * (*fraction - '0');
            scale /= 10;
            fraction++;
        }
    }

    if (abs(db) > (INT32_MAX - mb) / 100000)
        return INT32_MIN;

    return db * 100000 + sign * mb;
}
