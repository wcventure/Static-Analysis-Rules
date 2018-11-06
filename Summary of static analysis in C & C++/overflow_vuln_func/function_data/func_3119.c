static void oledate_to_iso8601(char *buf, int buf_size, int64_t value)
{
    time_t t = 631112400LL + 86400*av_int2dbl(value);
    strftime(buf, buf_size, "%Y-%m-%d %H:%M:%S", gmtime(&t));
}
