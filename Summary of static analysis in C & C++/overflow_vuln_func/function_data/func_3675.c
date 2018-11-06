static int read_ts(const char *s, int64_t *start, int *duration)
{
    int64_t end;
    int hh1, mm1, ss1, ms1;
    int hh2, mm2, ss2, ms2;

    if (sscanf(s, "%u:%u:%u.%u,%u:%u:%u.%u",
               &hh1, &mm1, &ss1, &ms1, &hh2, &mm2, &ss2, &ms2) == 8) {
        end    = (hh2*3600 + mm2*60 + ss2) * 100 + ms2;
        *start = (hh1*3600 + mm1*60 + ss1) * 100 + ms1;
        *duration = end - *start;
        return 0;
    }
    return -1;
}
