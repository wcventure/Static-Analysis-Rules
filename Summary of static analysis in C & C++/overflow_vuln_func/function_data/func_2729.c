static int read_cpuinfo(const char *field, char *value, int len)
{
    FILE *f;
    int ret = -1;
    int field_len = strlen(field);
    char line[512];

    f = fopen("/proc/cpuinfo", "r");
    if (!f) {
        return -1;
    }

    do {
        if(!fgets(line, sizeof(line), f)) {
            break;
        }
        if (!strncmp(line, field, field_len)) {
            strncpy(value, line, len);
            ret = 0;
            break;
        }
    } while(*line);

    fclose(f);

    return ret;
}
