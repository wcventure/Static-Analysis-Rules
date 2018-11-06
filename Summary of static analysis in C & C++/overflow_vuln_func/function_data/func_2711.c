static uint64_t kvmppc_read_int_cpu_dt(const char *propname)
{
    char buf[PATH_MAX];
    union {
        uint32_t v32;
        uint64_t v64;
    } u;
    FILE *f;
    int len;

    if (kvmppc_find_cpu_dt(buf, sizeof(buf))) {
        return -1;
    }

    strncat(buf, "/", sizeof(buf) - strlen(buf));
    strncat(buf, propname, sizeof(buf) - strlen(buf));

    f = fopen(buf, "rb");
    if (!f) {
        return -1;
    }

    len = fread(&u, 1, sizeof(u), f);
    fclose(f);
    switch (len) {
    case 4:
        /
        return be32_to_cpu(u.v32);
    case 8:
        return be64_to_cpu(u.v64);
    }

    return 0;
}
