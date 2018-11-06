static int64_t read_ts(const char *s)
{
    int hh, mm, ss, ms;
    if (sscanf(s, "%u:%u:%u.%u", &hh, &mm, &ss, &ms) == 4) return (hh*3600 + mm*60 + ss) * 1000 + ms;
    if (sscanf(s,    "%u:%u.%u",      &mm, &ss, &ms) == 3) return (          mm*60 + ss) * 1000 + ms;
    return AV_NOPTS_VALUE;
}
